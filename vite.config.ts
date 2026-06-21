import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import {
  generateEnumsPlugin,
  wasmTypesPlugin,
} from "./scripts/gen-wasm-types.ts";
import path from "path";
// https://vite.dev/config/
export default defineConfig({
  plugins: [
    generateEnumsPlugin([
      path.resolve(__dirname, "./include/events.hpp"),
      path.resolve(__dirname, "./include/scene_registry.hpp"),
    ]),
    wasmTypesPlugin(),
    react(),
  ],
  base: "/algoplex/",
});

