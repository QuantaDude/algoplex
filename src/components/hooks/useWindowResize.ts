import { useEffect, useState, type RefObject } from "react";
import type { MainModule } from "../../types/wasmmodule";

export default function useWindowSize(wasmModule: RefObject<MainModule>) {
  const [size, setSize] = useState({
    w: window.innerWidth,
    h: window.innerHeight,
  });
  useEffect(() => {
    const handler = () => {
      setSize({ w: window.innerWidth, h: window.innerHeight });
      wasmModule.current._on_resize();
    };
    window.addEventListener("resize", handler);
    return () => window.removeEventListener("resize", handler);
  }, []);
  return size;
}

