import tkinter as tk
from tkinter import scrolledtext, ttk, messagebox, filedialog
import subprocess
import tempfile
import os
import re
import sys

class CompilerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("My Language Compiler IDE")
        self.root.geometry("1200x800")
        self.root.configure(bg="#F0F0F0")

        # --- Color Scheme ---
        self.colors = {
            "primary": "#4A3AFF",
            "primary_text": "#FFFFFF",
            "background": "#F0F0F0",
            "frame": "#FFFFFF",
            "tab_bg": "#E6E6FA",
            "tab_selected": "#4A3AFF",
            "tab_selected_text": "#FFFFFF",
            "tab_hover": "#B3A7F7",
        }

        # --- Style Configuration ---
        style = ttk.Style(self.root)
        style.theme_use("clam")
        
        # Style for the Button
        style.configure("TButton",
            background=self.colors["primary"],
            foreground=self.colors["primary_text"],
            font=("Helvetica", 12, "bold"),
            bordercolor=self.colors["primary"],
            lightcolor=self.colors["primary"],
            darkcolor=self.colors["primary"],
            padding=(16, 8),
            relief="flat"
        )
        style.map("TButton",
            background=[("active", "#5A4AFF"), ("pressed", self.colors["primary"])],
            relief=[("pressed", "flat"), ("!pressed", "flat")],
            padding=[("active", (16, 8)), ("pressed", (16, 8)), ("!pressed", (16, 8))]
        )

        # Style for the Tabs
        style.configure("TNotebook", background=self.colors["tab_bg"], borderwidth=0)
        style.configure("TNotebook.Tab",
                        background=self.colors["tab_bg"],
                        foreground="#222",
                        padding=[28, 12],
                        font=("Helvetica", 14, "bold"),
                        borderwidth=0)
        style.map("TNotebook.Tab",
                  background=[("selected", self.colors["tab_selected"]),
                              ("active", self.colors["tab_hover"])],
                  foreground=[("selected", self.colors["tab_selected_text"]),
                              ("active", "#222")],
                  padding=[("selected", [28, 12]), ("active", [28, 12]), ("!selected", [28, 12])])

        # --- Header ---
        self.header = tk.Frame(root, bg=self.colors["primary"], height=50)
        self.header.pack(fill=tk.X, side=tk.TOP)
        self.header_label = tk.Label(self.header, text="My Language Compiler IDE", bg=self.colors["primary"], fg=self.colors["primary_text"], font=("Helvetica", 16, "bold"))
        self.header_label.pack(pady=10)

        # --- Main Layout ---
        self.main_pane = tk.PanedWindow(root, orient=tk.HORIZONTAL, bg=self.colors["background"], sashwidth=8, sashrelief=tk.RAISED)
        self.main_pane.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # --- Left Pane (Editor and Buttons) ---
        self.left_pane = tk.Frame(self.main_pane, bg=self.colors["frame"])
        
        self.editor_label = ttk.Label(self.left_pane, text="Code Editor", font=("Helvetica", 12, "bold"), background=self.colors["frame"])
        self.editor_label.pack(anchor="w", padx=10, pady=(10, 5))
        
        self.code_editor = scrolledtext.ScrolledText(self.left_pane, wrap=tk.WORD, width=80, height=25, font=("Courier New", 12), relief=tk.SOLID, borderwidth=1)
        self.code_editor.pack(fill=tk.BOTH, expand=True, padx=10)
        self.code_editor.insert("1.0", "// Write your code here\npublish(\"taniya\");")

        # --- Button Row ---
        self.button_row = tk.Frame(self.left_pane, bg=self.colors["frame"])
        self.button_row.pack(fill=tk.X, padx=10, pady=(5, 10))
        self.upload_button = ttk.Button(self.button_row, text="Upload File", command=self.open_file, style="TButton")
        self.upload_button.pack(side=tk.LEFT, padx=(0, 10))
        self.compile_button = ttk.Button(self.button_row, text="Compile and Run", command=self.compile_code, style="TButton")
        self.compile_button.pack(side=tk.LEFT)
        
        self.main_pane.add(self.left_pane, stretch="always")

        self.output_frame = tk.Frame(self.main_pane, bg=self.colors["frame"])
        self.notebook = ttk.Notebook(self.output_frame, style="TNotebook")
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.main_pane.add(self.output_frame, stretch="always")

        self.tabs = {}
        phase_names = [
            "Lexical", "Syntax", "Semantic", "IR", "Optimization", "CodeGen"
        ]
        # Increase all output font sizes by 2 units
        font_sizes = {"Lexical": 11, "Syntax": 14, "Semantic": 11, "IR": 9, "Optimization": 9, "CodeGen": 9}
        for name in phase_names:
            tab_frame = tk.Frame(self.notebook, bg=self.colors["frame"])
            self.notebook.add(tab_frame, text=name)
            font_family = "Courier New"
            font_size = font_sizes.get(name, 9)
            text_area = scrolledtext.ScrolledText(tab_frame, wrap=tk.WORD, font=(font_family, font_size), state='disabled', relief=tk.FLAT, bg="#FAFAFF")
            text_area.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
            self.tabs[name] = text_area
        
        # Set initial message
        self.tabs["Lexical"].config(state='normal')
        self.tabs["Lexical"].insert("1.0", "No output yet.")
        self.tabs["Lexical"].config(state='disabled')


    def clear_tabs(self):
        for text_area in self.tabs.values():
            text_area.config(state='normal')
            text_area.delete('1.0', tk.END)
            text_area.config(state='disabled')

    def open_file(self):
        file_path = filedialog.askopenfilename(
            title="Open Code File",
            filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")]
        )
        if file_path:
            try:
                with open(file_path, "r", encoding="utf-8") as f:
                    code = f.read()
                self.code_editor.delete("1.0", tk.END)
                self.code_editor.insert("1.0", code)
            except Exception as e:
                messagebox.showerror("File Error", f"Could not open file:\n{e}")

    def compile_code(self):
        self.clear_tabs()
        
        code = self.code_editor.get("1.0", tk.END)
        if not code.strip():
            messagebox.showwarning("Empty Code", "There is no code to compile.")
            return

        # --- Windows compatibility: use compiler.exe on Windows ---
        if sys.platform.startswith("win"):
            compiler_path = os.path.join(os.getcwd(), "compiler.exe")
        else:
            compiler_path = os.path.join(os.getcwd(), "compiler")

        if not os.path.exists(compiler_path) or not os.access(compiler_path, os.X_OK):
            messagebox.showerror("Compiler Not Found", 
                                 f"The compiler executable was not found at '{compiler_path}'.\n"
                                 f"Please make sure you have compiled the C project with 'make'.")
            return

        try:
            with tempfile.NamedTemporaryFile(mode='w+', delete=False, suffix=".mylang", encoding='utf-8') as tmp_file:
                tmp_file.write(code)
                tmp_file_path = tmp_file.name

            process = subprocess.run(
                [compiler_path, tmp_file_path],
                capture_output=True,
                text=True,
                timeout=10
            )
            self.parse_and_display(process.stdout, process.stderr)

        except Exception as e:
            messagebox.showerror("Error", f"An unexpected error occurred: {e}")
        finally:
            if 'tmp_file_path' in locals() and os.path.exists(tmp_file_path):
                os.remove(tmp_file_path)

    def parse_and_display(self, stdout, stderr):
        # Handle stderr first, as it indicates a more severe issue
        if stderr:
            messagebox.showerror("Compiler Error", stderr)
            if "Segmentation fault" in stderr:
                messagebox.showerror("Compiler Crash", "The C compiler crashed with a Segmentation Fault.")
            return

        # Check for actual compiler errors in stdout, not just status messages
        error_line = None
        for line in stdout.splitlines():
            lower_line = line.lower()
            if "error" in lower_line:
                # Ignore messages that indicate no errors were found
                if "no " in lower_line or "0 " in lower_line or "âœ“" in line or "✓" in line:
                    continue
                # Found a real error line
                error_line = line
                break
        
        if error_line:
            messagebox.showerror("Compiler Error", error_line.strip())
            return  # Stop processing to avoid showing phase output

        phase_pattern = r"={2,}\n Phase: (.*?)\n={2,}\n"
        sections = re.split(phase_pattern, stdout, flags=re.DOTALL)
        
        tab_mapping = {
            "Lexical and Syntax Analysis": "Lexical",
            "Syntax Analysis (AST)": "Syntax",
            "Semantic Analysis": "Semantic",
            "Intermediate Representation (IR)": "IR",
            "Code Generation": "CodeGen"
        }
        
        output_displayed = False
        
        # Display all phases except Optimization
        if len(sections) > 1:
            output_displayed = True
            it = iter(sections[1:])
            for title in it:
                content = next(it, "").strip()
                mapped_title = tab_mapping.get(title.strip())
                if mapped_title:
                    self.tabs[mapped_title].config(state='normal')
                    if mapped_title == "Syntax":
                        try:
                            content = content.encode('utf-8').decode('utf-8')
                        except Exception:
                            pass
                    self.tabs[mapped_title].insert(tk.END, content)
                    self.tabs[mapped_title].config(state='disabled')
        
        # --- Handle Detailed Optimization Output Separately ---
        if "--- OPTIMIZER START ---" in stdout:
            output_displayed = True
            applied_lines = [line for line in stdout.splitlines() if "Applied:" in line]
            optimization_summary = "\n".join(applied_lines) if applied_lines else "No optimizations were applied."
            optimized_ir = ""
            if "--- OPTIMIZED IR ---" in stdout:
                start_marker = "--- OPTIMIZED IR ---\n"
                end_marker = "--- OPTIMIZER END ---"
                try:
                    start_index = stdout.index(start_marker) + len(start_marker)
                    end_index = stdout.index(end_marker, start_index)
                    optimized_ir = stdout[start_index:end_index].strip()
                except ValueError:
                    optimized_ir = "Could not parse optimized IR."
            self.tabs["Optimization"].config(state='normal')
            self.tabs["Optimization"].insert(tk.END, "Applied Optimizations:\n" + optimization_summary + "\n\n")
            if optimized_ir:
                self.tabs["Optimization"].insert(tk.END, "Final Optimized Code:\n" + optimized_ir)
            self.tabs["Optimization"].config(state='disabled')
        
        # Handle cases where no phases were displayed, but there is output (e.g., from a simple publish)
        if not output_displayed and stdout.strip():
            messagebox.showinfo("Compiler Output", stdout)

    def on_close(self):
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = CompilerGUI(root)
    root.protocol("WM_DELETE_WINDOW", app.on_close)
    root.mainloop() 