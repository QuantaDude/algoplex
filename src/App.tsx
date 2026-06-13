import { useEffect, useRef, useState, type RefObject } from "react";
import { saveAs } from "file-saver";
import AlgoVisualizer from "./wasm/algo-visualizer.js";
import "./App.css";
import Navbar from "./components/navbar/Navbar.js";
import AlgoMenuPanel from "./components/algo-menu-panel/AlgoMenuPanel.js";
import SettingsPanel from "./components/settings-panel/SettingsPanel.js";
import type { MainModule } from "./types/wasmmodule.d.ts";
import InfoPanel from "./components/info-panel/InfoPanel.tsx";
import CodePanel from "./components/code-panel/CodePanel.tsx";
import Tooltip, { type TooltipPage } from "./components/tooltip/Tooltip.tsx";

function App() {
  const moduleRef: RefObject<MainModule | null> = useRef(null);
  const hasInitialized: RefObject<boolean> = useRef(false);
  const [showSidebar, setShowSidebar] = useState(true);

  const navbarRef: RefObject<HTMLElement> = useRef(null);
  const algoMenuPanelRef: RefObject<HTMLElement> = useRef(null);
  const settingsPanelRef: RefObject<HTMLElement> = useRef(null);
  const codePanelRef: RefObject<HTMLElement> = useRef(null);

  const infoPanel1Ref: RefObject<HTMLElement> = useRef(null);
  const canvasRef: RefObject<HTMLElement> = useRef(null);

  const pages: TooltipPage[] = [
    {
      title: "Algorithm List",
      text: [
        "This panel contains all the algorithms you can try out in Algo Visualizer.",
        "You can also select the `custom algorithm` to write your own algorithm.",
      ],
      newLoc: true,
      tooltipTarget: algoMenuPanelRef,
      tooltipPos: { x: 250, y: -100 },
      arrowRotation: 90,
      arrowRelDistance: { x: -5, y: 10 },
    },
    {
      title: "Algorithm List",
      text: [
        "For now, you may pick one algorithm from the Graph/Tree list.",
        "Pick one.",
      ],
      newLoc: false,
      tooltipTarget: algoMenuPanelRef,
      tooltipPos: { x: 250, y: -100 },
      arrowRotation: 90,
      arrowRelDistance: { x: -5, y: 10 },
    },
    {
      title: "Controls Panel",
      text: [
        "This Panel has all the input modes for the canvas, and settings for the algorithm scene.",
        "All the controls and settings for the selected algorithm are here.",
      ],
      newLoc: true,
      tooltipTarget: settingsPanelRef,
      tooltipPos: { x: -700, y: 0 },
      arrowRotation: -90,
      arrowRelDistance: { x: 25, y: 10 },
    },
    {
      title: "Code Editor",
      text: [
        "This is where the algorithm's code is displayed and then highlighted step wise during execution",
      ],
      newLoc: true,
      tooltipTarget: codePanelRef,
      tooltipPos: { x: 0, y: -700 },
      arrowRotation: 0,
      arrowRelDistance: { x: 10, y: 25 },
    },
    {
      title: "DFS Stack",
      text: [
        "This is the stack view.",
        "Here you will be able the nodes which will get pushed into the stack array.",
      ],
      newLoc: true,
      tooltipTarget: infoPanel1Ref,
      tooltipPos: { x: 200, y: -500 },
      arrowRotation: 45,
      arrowRelDistance: { x: -5, y: 20 },
    },
  ];

  useEffect(() => {
    if (hasInitialized.current) return;
    hasInitialized.current = true;

    window.addEventListener(
      "register_algorithms",
      (e: CustomEvent) => {
        console.log(e.detail); // e.detail is the parsed array
      },
      { once: true },
    );
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
      <AlgoMenuPanel ref={algoMenuPanelRef} />
      <canvas
        id="canvas"
        style={{ width: "100%", height: "100%" }}
        onContextMenu={(e) => e.preventDefault()}
      />
      <Tooltip pages={pages} />
      <SettingsPanel ref={settingsPanelRef} wasmModule={moduleRef!} />
      <InfoPanel ref={infoPanel1Ref} id="info1panel" />
      <CodePanel ref={codePanelRef}></CodePanel>
      <InfoPanel id="info2panel" />
    </>
  );
}

export default App;

