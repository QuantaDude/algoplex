// scripts/generateEnums.ts
import fs from "fs";
import path from "path";
import { Plugin } from "vite";

interface EnumMember {
  value: number;
  label: string; // comment if present, else identifier
}

type ParsedEnum = Record<string, EnumMember>;

function parseEnums(hpp: string): Record<string, ParsedEnum> {
  const enums: Record<string, ParsedEnum> = {};
  const enumRegex = /enum\s+class\s+(\w+)\s*(?::\s*\w+)?\s*\{([^}]+)\}/g;
  let match;

  while ((match = enumRegex.exec(hpp)) !== null) {
    const name = match[1];
    const body = match[2];
    enums[name] = {};
    let counter = 0;

    for (const line of body.split("\n")) {
      // extract optional inline comment
      const commentMatch = line.match(/\/\/\s*(.+)$/);
      const label = commentMatch?.[1].trim();

      // strip comment, then strip trailing comma and whitespace
      const cleaned = line
        .replace(/\/\/.*$/, "")
        .replace(/,\s*$/, "")
        .trim();
      if (!cleaned) continue;

      const [ident, val] = cleaned.split("=").map((s) => s.trim());
      if (!ident) continue;

      // skip sentinel values
      if (ident.endsWith("_COUNT")) continue;

      counter = val !== undefined ? parseInt(val) : counter;
      enums[name][ident] = { value: counter, label: label ?? ident };
      counter++;
    }
  }
  return enums;
}

function generateTs(
  enums: Record<string, ParsedEnum>,
  sourceName: string,
): string {
  const lines = [
    `// AUTO-GENERATED — do not edit. Source: include/${sourceName}`,
    "",
  ];

  for (const [name, members] of Object.entries(enums)) {
    const entries = Object.entries(members);

    // value object: Foo = { NodeAdd: 0, EdgeAdd: 1, ... }
    lines.push(`export const ${name} = {`);
    for (const [key, { value }] of entries) {
      lines.push(`  ${key}: ${value},`);
    }
    lines.push(`} as const;`);
    lines.push(`export type ${name} = typeof ${name}[keyof typeof ${name}];`);
    lines.push("");

    // label map: FooLabel = { 0: "Graph / Tree", 1: "Sort", ... }
    lines.push(`export const ${name}Label: Record<${name}, string> = {`);
    for (const [, { value, label }] of entries) {
      lines.push(`  [${value}]: "${label}",`);
    }
    lines.push(`};`);
    lines.push("");
  }

  return lines.join("\n");
}

export function generateEnumsPlugin(hppPaths: string[]): Plugin {
  const outDir = path.resolve(__dirname, "../src/types");

  function run(hppPath: string) {
    const sourceName = path.basename(hppPath);
    const outName = sourceName.replace(/\.hpp$/, ".ts");
    const outPath = path.join(outDir, outName);

    const hpp = fs.readFileSync(hppPath, "utf-8");
    const enums = parseEnums(hpp);
    const output = generateTs(enums, sourceName);

    fs.mkdirSync(outDir, { recursive: true });
    fs.writeFileSync(outPath, output);
    console.log(`[generateEnums] src/types/${outName} updated`);
  }

  return {
    name: "generate-enums",
    buildStart() {
      for (const p of hppPaths) run(p);
    },
    configureServer(server) {
      for (const p of hppPaths) server.watcher.add(p);
      server.watcher.on("change", (file) => {
        if (hppPaths.includes(file)) run(file);
      });
    },
  };
}

