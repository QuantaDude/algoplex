import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { generateEnumsPlugin } from "./scripts/generateEnums.ts";
import path from "path";
// https://vite.dev/config/
export default defineConfig({
  plugins: [
    generateEnumsPlugin([
      path.resolve(__dirname, "./include/events.hpp"),
      path.resolve(__dirname, "./include/scene_registry.hpp"),
    ]),
    react(),
  ],
  base: "/algo-visualizer/",
});

