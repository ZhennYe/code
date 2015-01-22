import os
from Tkinter import *
import ttk

class UI(Frame):
    def __init__(self,master):
        Frame.__init__(self, master)
        self.master = master
        self.initUI()

    def initUI(self):
      self.popup = popup = Toplevel(self)
      Label(popup, text="Please wait until the file is created").grid(
          row=0, column=0)
      self.progressbar = progressbar = ttk.Progressbar(popup,
          orient=HORIZONTAL, length=200, mode='indeterminate')
      progressbar.grid(row=1, column=0)
      progressbar.start()
      self.checkfile()


    def checkfile(self):
      if os.path.exists("myfile.txt"):
        print('found it')
        self.progressbar.stop()
        self.popup.destroy()
      else:
        print('not created yet')
        self.after(100, self.checkfile) # Call this method after 100 ms.

if __name__ == "__main__":
    root = Tk()
    aplicacion = UI(root)
    root.mainloop()




