import { useEffect, useRef, useState, type RefObject } from "react";
import { saveAs } from "file-saver";
import AlgoVisualizer from "./wasm/algo-visualizer.js";
import "./App.css";
import Navbar from "./components/navbar/Navbar.js";
import AlgoMenuPanel from "./components/algo-menu-panel/AlgoMenuPanel.js";
import SettingsPanel from "./components/settings-panel/SettingsPanel.js";
import type { MainModule } from "./types/wasmmodule.d.ts";

function App() {
  const moduleRef: RefObject<MainModule | null> = useRef(null);
  const hasInitialized: RefObject<boolean> = useRef(false);
  const [showSidebar, setShowSidebar] = useState(true);
  useEffect(() => {
    if (hasInitialized.current) return;
    hasInitialized.current = true;

    AlgoVisualizer({
      canvas: (function () {
        var canvas = document.getElementById("canvas");

        // As a default initial behavior, pop up an alert when webgl context is lost.
        // To make your application robust, you may want to override this behavior before shipping!
        // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
        canvas.addEventListener(
          "webglcontextlost",
          function (e) {
            alert("WebGL context lost. You will need to reload the page.");
            e.preventDefault();
          },
          false,
        );

        return canvas;
      })(),

      print: (text: string) => console.log(text),
      printErr: (text: string) => console.error(text),
      onAbort: () => console.error("WASM aborted"),
      // setStatus: (text: string) => console.log(text),
      preRun: [],
      postRun: [],
      // This tells Emscripten's preload plugin to skip image decoding
      // Raylib loads images itself from the VFS — it doesn't need them pre-decoded
      // noImageDecodingc> true,
      // noAudioDecoding: true,
      locateFile: (path: string) => `${import.meta.env.BASE_URL}wasm/${path}`,
    }).then((module: MainModule) => {
      moduleRef.current = module;

      const navbar = Navbar({ wasmModule: moduleRef! });

      // window.saveFileFromMEMFSToDisc> = (
      //   memoryFSname: string,
      //   localFSname: string,
      // ) => {
      //   const data = module.FS.readFile(memoryFSname);
      //   const blob = new Blob([data.buffer], {
      //     type: "application/octet-binary",
      //   });
      //   saveAs(blob, localFSname);
      // };
    });
  }, []);

  return (
    <>
      <Navbar wasmModule={moduleRef!} />
      <AlgoMenuPanel />
      <textarea id="output"></textarea>
      <canvas
        id="canvas"
        style={{ width: "100%", height: "100%" }}
        onContextMenu={(e) => e.preventDefault()}
      />
      <SettingsPanel wasmModule={moduleRef!} />
    </>
  );
}

export default App;

