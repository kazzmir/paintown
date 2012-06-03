package com.rafkind.paintown

/* Stores actions to undo. Each action has a description associated with it as a string.
 * Actions popped off the undo stack are added to the redo stack and vice-versa.
 *
 * For now infinite undo is offered.
 */

object Undo{
  type thunk = () => Unit

  var undos = List[Undo]()
  var redos = List[Undo]()
  def addUndo(undo: thunk, what:String):Unit = {
    undos = Undo(undo, what) :: undos
  }

  def popUndo():Unit = {
    if (! undos.isEmpty){
      val first = undos.head
      undos = undos.tail
      first.doit()
      redos = first :: redos
    }
  }

  def popRedo():Unit = {
    if (! redos.isEmpty){
      val first = redos.head
      redos = redos.tail
      first.doit()
      undos = first :: undos
    }
  }
}

case class Undo(doit:Undo.thunk, what:String)
