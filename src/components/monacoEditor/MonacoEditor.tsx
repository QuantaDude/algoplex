import { Editor, type EditorProps } from "@monaco-editor/react";
import { useState } from "react";
const EditorOptions = {
  automaticLayout: true,
  contextmenu: true,
  dragAndDrop: true,
  dropIntoEditor: {
    enabled: true,
  },
  detectIndentation: false,
  fixedOverflowWidgets: false,
  fontFamily: "Fira Mono",
  fontSize: 16,
  formatOnPaste: true,
  formatOnType: true,
  lineHeight: 1.5,
  minimap: {
    enabled: true,
  },
  padding: {
    top: 8,
    button: 8,
  },
  scrollbar: {
    verticalScrollbarSize: 9,
    horizontalScrollbarSize: 9,
    alwaysConsumeMouseWheel: false,
  },
  scrollBeyondLastLine: false,
  smoothScrolling: true,
  tabSize: 4,
  quickSuggestions: true,
  // wordBasedSuggestions: true,
};

export default function MonacoEditor({ code }: { code: string }) {
  const [editorCode, setEditorCode] = useState(code);

  return (
    <>
      <Editor
        options={EditorOptions}
        value={editorCode}
        theme="vs-dark"
        language="python"
        onChange={(value, e) => setEditorCode(value ?? "")}
      />
    </>
  );
}

