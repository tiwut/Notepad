import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext
import os

class TextEditor:
    """
    A class that implements a professional, modern text editor (Notepad) using Tkinter.
    Key features: Dark Theme, line numbers, and an improved status bar.
    """
    def __init__(self, master):
        self.master = master
        self.master.title("Untitled - Python Notepad")
        
        self.EDITOR_BG = "#282c34"
        self.EDITOR_FG = "#abb2bf"
        self.LINE_NUM_BG = "#3e4451"
        self.LINE_NUM_FG = "#61afef"
        self.STATUS_BG = "#3e4451"
        self.STATUS_FG = "#ffffff"
        
        self.current_file = None
        
        self.master.geometry("1000x700")
        self.master.protocol("WM_DELETE_WINDOW", self._on_closing) 

        self.text_frame = tk.Frame(self.master, bg=self.EDITOR_BG)
        self.text_frame.pack(expand=True, fill='both')

        self.linenumbers = tk.Text(
            self.text_frame,
            width=4,
            padx=5,
            bd=0,
            bg=self.LINE_NUM_BG,
            fg=self.LINE_NUM_FG,
            font=("Consolas", 12),
            state='disabled',
            wrap='none',
            relief=tk.FLAT
        )
        self.linenumbers.pack(side=tk.LEFT, fill=tk.Y)
        
        self.text_area = scrolledtext.ScrolledText(
            self.text_frame, 
            wrap=tk.WORD, 
            font=("Consolas", 12),
            undo=True,
            bg=self.EDITOR_BG,
            fg=self.EDITOR_FG,
            insertbackground="#ffffff",
            relief=tk.FLAT,
            padx=10,
            pady=10,
            insertwidth=3
        )
        self.text_area.pack(side=tk.RIGHT, expand=True, fill='both')
        
        self._create_menu_bar()
        self._create_status_bar()

        self.text_area.bind('<<Modified>>', self._update_status)
        self.text_area.edit_modified(False)
        
        self.master.bind('<Control-n>', lambda event: self._new_file())
        self.master.bind('<Control-o>', lambda event: self._open_file())
        self.master.bind('<Control-s>', lambda event: self._save_file())
        self.master.bind('<Control-S>', lambda event: self._save_file_as())
        self.master.bind('<Control-z>', lambda event: self.text_area.edit_undo())
        self.master.bind('<Control-y>', lambda event: self.text_area.edit_redo())

        self.text_area.bind('<Key>', lambda e: self.master.after_idle(self._update_line_numbers))
        self.text_area.bind('<Button-1>', lambda e: self.master.after_idle(self._update_line_numbers))
        self.text_area.bind('<MouseWheel>', lambda e: self.master.after_idle(self._update_line_numbers))
        
        self._update_line_numbers()
        self._update_status()
        
    def _create_menu_bar(self):
        """Creates the menu bar with File and Edit menus."""
        menu_bar = tk.Menu(self.master)
        self.master.config(menu=menu_bar)

        file_menu = tk.Menu(menu_bar, tearoff=0)
        menu_bar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New", command=self._new_file, accelerator="Ctrl+N")
        file_menu.add_command(label="Open...", command=self._open_file, accelerator="Ctrl+O")
        file_menu.add_separator()
        file_menu.add_command(label="Save", command=self._save_file, accelerator="Ctrl+S")
        file_menu.add_command(label="Save As...", command=self._save_file_as, accelerator="Ctrl+Shift+S")
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self._on_closing)

        edit_menu = tk.Menu(menu_bar, tearoff=0)
        menu_bar.add_cascade(label="Edit", menu=edit_menu)
        
        edit_menu.add_command(label="Undo", command=self.text_area.edit_undo, accelerator="Ctrl+Z")
        edit_menu.add_command(label="Redo", command=self.text_area.edit_redo, accelerator="Ctrl+Y")
        edit_menu.add_separator()
        
        edit_menu.add_command(label="Cut", command=lambda: self.text_area.event_generate("<<Cut>>"), accelerator="Ctrl+X")
        edit_menu.add_command(label="Copy", command=lambda: self.text_area.event_generate("<<Copy>>"), accelerator="Ctrl+C")
        edit_menu.add_command(label="Paste", command=lambda: self.text_area.event_generate("<<Paste>>"), accelerator="Ctrl+V")
    
    def _create_status_bar(self):
        """Creates a professional status bar in dark mode."""
        self.status_bar = tk.Label(
            self.master, 
            text="Ready | Line 1, Column 0", 
            bd=0, 
            relief=tk.FLAT, 
            anchor=tk.W,
            bg=self.STATUS_BG,
            fg=self.STATUS_FG,
            padx=10,
            pady=3
        )
        self.status_bar.pack(fill=tk.X, side=tk.BOTTOM)
        
        self.text_area.bind('<KeyRelease>', self._update_status)
        self.text_area.bind('<ButtonRelease-1>', self._update_status)
        self.text_area.bind('<Motion>', self._update_status)

    def _update_line_numbers(self, event=None):
        """Updates the line numbers and synchronizes scrolling."""
        
        final_line = self.text_area.index(tk.END).split('.')[0]
        line_count = int(final_line)
        
        linenumbers_text = "\n".join(str(i) for i in range(1, line_count))
        
        self.linenumbers.config(state=tk.NORMAL)
        self.linenumbers.delete(1.0, tk.END)
        self.linenumbers.insert(1.0, linenumbers_text)
        self.linenumbers.config(state=tk.DISABLED)

        dline = self.text_area.dlineinfo(1.0)
        if dline:
            self.linenumbers.yview_moveto(dline[1] / self.text_area.winfo_height())
            
        self.master.after(50, self._update_line_numbers)

    def _is_text_modified(self):
        """Checks if the text area has been modified since the last save/open."""
        return self.text_area.edit_modified()
    
    def _update_status(self, event=None):
        """Updates the status bar (Line/Column) and the window title."""
        
        try:
            line, column = self.text_area.index(tk.INSERT).split('.')
            status_text = f"Ready | Line {line}, Column {column}"
        except:
            status_text = "Ready"
        
        self.status_bar.config(text=status_text)
        self._update_title()
        
        self.text_area.edit_modified(False)

    def _update_title(self):
        """Updates the window title and adds an '*' if there are unsaved changes."""
        title = "Untitled"
        if self.current_file:
            title = os.path.basename(self.current_file)
            
        if self._is_text_modified():
            title = f"*{title}"
            
        self.master.title(f"{title} - Python Notepad")

    def _on_closing(self):
        """Handler for closing the window with a save prompt."""
        if self._check_unsaved_changes():
            self.master.destroy()

    def _check_unsaved_changes(self):
        """Asks the user if they want to save if there are changes."""
        if self._is_text_modified():
            response = messagebox.askyesnocancel(
                "Unsaved Changes",
                "Do you want to save the changes?"
            )
            if response is True:
                return self._save_file()
            elif response is False:
                return True
            else:
                return False
        return True

    def _new_file(self):
        """Creates a new, empty file."""
        if not self._check_unsaved_changes():
            return
            
        self.text_area.delete(1.0, tk.END)
        self.current_file = None
        self._update_title()
        self.text_area.edit_modified(False)
        self._update_status()
        self._update_line_numbers()

    def _open_file(self):
        """Opens an existing file."""
        if not self._check_unsaved_changes():
            return
            
        filepath = filedialog.askopenfilename(
            defaultextension=".txt",
            filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")]
        )
        
        if not filepath:
            return

        try:
            with open(filepath, "r", encoding="utf-8") as input_file:
                text = input_file.read()
            
            self.text_area.delete(1.0, tk.END)
            self.text_area.insert(1.0, text)
            self.current_file = filepath
            self._update_title()
            self.text_area.edit_modified(False)
            self._update_status()
            self._update_line_numbers()

        except Exception as e:
            messagebox.showerror("Error", f"Could not open file:\n{e}")
            self.current_file = None
            return False
        return True


    def _save_file(self):
        """Saves the current file. Uses Save As if not already saved."""
        if self.current_file:
            try:
                content = self.text_area.get(1.0, tk.END)
                with open(self.current_file, "w", encoding="utf-8") as output_file:
                    output_file.write(content)
                self.text_area.edit_modified(False)
                self._update_title()
                self._update_status()
                return True
            except Exception as e:
                messagebox.showerror("Error", f"Could not save file:\n{e}")
                return False
        else:
            return self._save_file_as()

    def _save_file_as(self):
        """Saves the current file under a new name."""
        filepath = filedialog.asksaveasfilename(
            defaultextension=".txt",
            filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")]
        )
        
        if not filepath:
            return False

        try:
            content = self.text_area.get(1.0, tk.END)
            with open(filepath, "w", encoding="utf-8") as output_file:
                output_file.write(content)
            
            self.current_file = filepath
            self._update_title()
            self.text_area.edit_modified(False)
            self._update_status()
            return True

        except Exception as e:
            messagebox.showerror("Error", f"Could not save file:\n{e}")
            return False


if __name__ == "__main__":
    root = tk.Tk()
    editor = TextEditor(root)
    root.mainloop()