package com.rafkind.paintown

/* Stores actions to undo. Each action has a description associated with it as a string.
 * Actions popped off the undo stack are added to the redo stack and vice-versa.
 *
 * For now infinite undo is offered.
 */

object Undo{
  type thunk = () => Unit

  /* Keep track of the menu item that will display the latest undo thing */
  var undoMenuItem:javax.swing.JMenuItem = null

  var undos = List[Undo]()
  var redos = List[Undo]()

  def setUndoMenuItem(menu:javax.swing.JMenuItem):Unit = {
    undoMenuItem = menu
  }

  def addUndo(what:String, undo: thunk):Unit = {
    undos.synchronized{
      if (undoMenuItem != null){
        undoMenuItem.setText("Undo: " + what)
      }
      undos = Undo(undo, what) :: undos
    }
  }

  def popUndo():Unit = {
    undos.synchronized{
      if (! undos.isEmpty){
        val first = undos.head
        undos = undos.tail
        first.doit()
        redos = first :: redos
        if (undoMenuItem != null){
          if (! undos.isEmpty){
            val newFirst = undos.head
            undoMenuItem.setText("Undo: " + newFirst.what)
          } else {
            undoMenuItem.setText("Undo last action")
          }
        }
      }
    }
  }

  def popRedo():Unit = {
    if (! redos.isEmpty){
      val first = redos.head
      redos = redos.tail
      first.doit()
      undos = first :: undos
      if (undoMenuItem != null){
        undoMenuItem.setText("Undo: " + first.what)
      }
    }
  }
}

case class Undo(doit:Undo.thunk, what:String)
