package com.rafkind.paintown.animator

import java.awt
import javax.swing

import com.rafkind.paintown._

class NewAnimator extends swing.JFrame("Paintown Animator"){
  def construct(){
    val screen = awt.Toolkit.getDefaultToolkit().getScreenSize()
    this.setSize((screen.getWidth() * 0.9).toInt,
                 (screen.getHeight() * 0.9).toInt);

    Closer.open()

    val menuBar = new swing.JMenuBar();
    val menuProgram = new swing.JMenu("Program");
    val levelEditor = new swing.JMenuItem("Run the level editor");
    val quit = new swing.JMenuItem("Quit");
    val clearCache = new swing.JMenuItem("Clear image cache");
    val data = new swing.JMenuItem("Data path");
    val closeTab = new swing.JMenuItem("Close Tab");
    menuProgram.add(levelEditor);
    menuProgram.add(data);
    menuProgram.add(clearCache);
    menuProgram.add(closeTab);
    menuProgram.add(quit);
    menuBar.add(menuProgram);

    val menuProjectile = new swing.JMenu("Projectile");
    menuBar.add(menuProjectile);

    val newProjectile = new swing.JMenuItem("New Projectile");
    menuProjectile.add(newProjectile);
    val openProjectile = new swing.JMenuItem("Open Projectile");
    menuProjectile.add(openProjectile);
    val saveProjectile = new swing.JMenuItem("Save Projectile");
    menuProjectile.add(saveProjectile);

    val menuCharacter = new swing.JMenu("Character");
    menuBar.add(menuCharacter);

    val newCharacter = new swing.JMenuItem("New Character");
    menuCharacter.add(newCharacter);
    val loadCharacter = new swing.JMenuItem("Open Character");
    menuCharacter.add(loadCharacter);
    val saveCharacter = new swing.JMenuItem("Save Character");
    menuCharacter.add(saveCharacter);
    val saveCharacterAs = new swing.JMenuItem("Save Character As");
    menuCharacter.add(saveCharacterAs);

    menuProgram.setMnemonic(awt.event.KeyEvent.VK_P);
    data.setMnemonic(awt.event.KeyEvent.VK_D);
    closeTab.setMnemonic(awt.event.KeyEvent.VK_C);
    closeTab.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_W, awt.Event.CTRL_MASK));
    quit.setMnemonic(awt.event.KeyEvent.VK_Q);
    quit.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_Q, awt.Event.CTRL_MASK));
    newCharacter.setMnemonic(awt.event.KeyEvent.VK_N);
    newCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_N, awt.Event.CTRL_MASK));
    menuCharacter.setMnemonic(awt.event.KeyEvent.VK_H);
    saveCharacter.setMnemonic(awt.event.KeyEvent.VK_S);
    saveCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_S, awt.Event.CTRL_MASK));
    saveCharacterAs.setMnemonic(awt.event.KeyEvent.VK_A);
    saveCharacterAs.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_A, awt.Event.CTRL_MASK));
    loadCharacter.setMnemonic(awt.event.KeyEvent.VK_O);
    loadCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_O, awt.Event.CTRL_MASK));

    this.setJMenuBar(menuBar);
    this.addWindowListener(new awt.event.WindowAdapter(){
      override def windowClosing(e:awt.event.WindowEvent){
        Closer.close();
      }
    });

    levelEditor.addActionListener(new swing.AbstractAction(){
      override def actionPerformed(event:awt.event.ActionEvent){
        com.rafkind.paintown.Editor2.main(new Array[String](0));
      }
    });

    quit.addActionListener(new awt.event.ActionListener() {
      override def actionPerformed(event:awt.event.ActionEvent){
        Closer.close()
      }
    });

    clearCache.addActionListener(new awt.event.ActionListener(){
      override def actionPerformed(event:awt.event.ActionEvent){
        MaskedImage.clearCache();
      }
    });
  }

  construct()
}

object Animator2{
  def main(args: Array[String]):Unit = {
    val editor = new NewAnimator();
    swing.SwingUtilities.invokeLater(new Runnable(){
        def run(){
          editor.setVisible(true);
        }
      });
  }
}
