import {
  useEffect,
  useMemo,
  useRef,
  useState,
  type RefObject,
} from "react";
import { saveAs } from "file-saver";
import AlgoVisualizer from "./wasm/algo-visualizer.js";
import Navbar from "./components/navbar/Navbar.js";
import AlgoMenuPanel, {
  type AlgoDescriptor,
} from "./components/algo-menu-panel/AlgoMenuPanel.js";
import SettingsPanel from "./components/settings-panel/SettingsPanel.js";
import type { MainModule } from "./types/wasmmodule.d.ts";
import InfoPanel from "./components/info-panel/InfoPanel.tsx";
import CodePanel from "./components/code-panel/CodePanel.tsx";
import Tooltip, { type TooltipPage } from "./components/tooltip/Tooltip.tsx";
import StackView from "./components/stackView/StackView.tsx";
import { createTooltipPages } from "./tooltips.ts";
import type { DFS_AFrame } from "./types/InfoPanel.ts";
import AdjacencyMatrix from "./components/adjMatrix/AdjacencyMatrix.tsx";
import { NodeDataInputHandler } from "./components/nodeDataInputModal/NodeDataInputHandler.tsx";
import useWindowSize from "./components/hooks/useWindowResize.ts";

function App() {
  const [currentStack, setStack] = useState<Array<DFS_AFrame>>([]);
  //wasmModule
  const moduleRef: RefObject<MainModule | null> = useRef(null);

  const hasInitialized: RefObject<boolean> = useRef(false);
  const [showTooltip, setShowTooltip] = useState(true);
  const [showSidebar, setShowSidebar] = useState(true);

  const navbarRef: RefObject<HTMLElement> = useRef(null);
  const algoMenuPanelRef: RefObject<HTMLElement> = useRef(null);
  const settingsPanelRef: RefObject<HTMLElement> = useRef(null);
  const codePanelRef: RefObject<HTMLElement> = useRef(null);

  const infoPanel1Ref: RefObject<HTMLElement> = useRef(null);
  const infoPanel2Ref: RefObject<HTMLElement> = useRef(null);
  const canvasRef: RefObject<HTMLElement> = useRef(null);
  const [refsReady, setRefsReady] = useState(false);

  useWindowSize(moduleRef, canvasRef);
  useEffect(() => {
    if (
      algoMenuPanelRef.current &&
      settingsPanelRef.current &&
      canvasRef.current &&
      infoPanel1Ref.current &&
      infoPanel2Ref.current &&
      codePanelRef.current &&
      navbarRef.current
    ) {
      setRefsReady(true);
    }
  }, []);
  const tooltipPages = useMemo(
    () =>
      createTooltipPages({
        algoMenuPanelRef,
        settingsPanelRef,
        navbarRef,
        canvasRef,
        infoPanel1Ref,
        infoPanel2Ref,
        codePanelRef,
      }),
    [],
  );

  function updateStack(stack: Array<string>) {
    setStack(stack);
  }

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
    });
  }, []);

  return (
    <>
      <Navbar ref={navbarRef} wasmModule={moduleRef!} />
      <AlgoMenuPanel ref={algoMenuPanelRef} />
      <canvas
        id="canvas"
        ref={canvasRef}
        style={{ width: "100%", height: "100%" }}
        onContextMenu={(e) => e.preventDefault()}
        onMouseEnter={() => moduleRef.current?._set_receive_inputs(true)}
        onMouseLeave={() => moduleRef.current?._set_receive_inputs(false)}
      >
        {moduleRef.current == null ? <h3>Loading...</h3> : undefined}
      </canvas>
      {refsReady && showTooltip && (
        <Tooltip
          pages={tooltipPages}
          onClose={() => {
            setShowTooltip(false);
          }}
        />
      )}
      {canvasRef.current != null && (
        <NodeDataInputHandler wasmModule={moduleRef} canvasRef={canvasRef} />
      )}
      <SettingsPanel ref={settingsPanelRef} wasmModule={moduleRef!} />
      <InfoPanel ref={infoPanel1Ref} id="info1panel" type="Stack">
        <StackView
          items={currentStack}
          onUpdate={updateStack}
          wasmModule={moduleRef!}
        />
      </InfoPanel>
      <CodePanel ref={codePanelRef}></CodePanel>
      <InfoPanel ref={infoPanel2Ref} id="info2panel" type="Graph">
        <AdjacencyMatrix wasmModule={moduleRef!} />
      </InfoPanel>
    </>
  );
}

export default App;

