package com.rafkind.paintown.animator

import java.awt.Toolkit
import javax.swing._

class NewAnimator extends JFrame("Paintown Animator"){
  def construct(){
    val screen = Toolkit.getDefaultToolkit().getScreenSize()
    this.setSize((screen.getWidth() * 0.9).toInt,
                 (screen.getHeight() * 0.9).toInt);
  }

  construct()
}

object Animator2{
  def main(args: Array[String]):Unit = {
    val editor = new NewAnimator();
    SwingUtilities.invokeLater(new Runnable(){
        def run(){
          editor.setVisible(true);
        }
      });
  }
}
