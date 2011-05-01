package com.rafkind.paintown.animator

import java.awt.Event
import java.awt.event._
import java.awt.Toolkit
import javax.swing.event._
import javax.swing._

import com.rafkind.paintown._

class NewAnimator extends JFrame("Paintown Animator"){
  def construct(){
    val screen = Toolkit.getDefaultToolkit().getScreenSize()
    this.setSize((screen.getWidth() * 0.9).toInt,
                 (screen.getHeight() * 0.9).toInt);

    Closer.open()

    val menuBar = new JMenuBar();
    val menuProgram = new JMenu("Program");
    val levelEditor = new JMenuItem("Run the level editor");
    val quit = new JMenuItem("Quit");
    val clearCache = new JMenuItem("Clear image cache");
    val data = new JMenuItem("Data path");
    val closeTab = new JMenuItem("Close Tab");
    menuProgram.add(levelEditor);
    menuProgram.add(data);
    menuProgram.add(clearCache);
    menuProgram.add(closeTab);
    menuProgram.add(quit);
    menuBar.add(menuProgram);

    val menuProjectile = new JMenu("Projectile");
    menuBar.add(menuProjectile);

    val newProjectile = new JMenuItem("New Projectile");
    menuProjectile.add(newProjectile);
    val openProjectile = new JMenuItem("Open Projectile");
    menuProjectile.add(openProjectile);
    val saveProjectile = new JMenuItem("Save Projectile");
    menuProjectile.add(saveProjectile);

    val menuCharacter = new JMenu("Character");
    menuBar.add(menuCharacter);

    val newCharacter = new JMenuItem("New Character");
    menuCharacter.add(newCharacter);
    val loadCharacter = new JMenuItem("Open Character");
    menuCharacter.add(loadCharacter);
    val saveCharacter = new JMenuItem("Save Character");
    menuCharacter.add(saveCharacter);
    val saveCharacterAs = new JMenuItem("Save Character As");
    menuCharacter.add(saveCharacterAs);

    menuProgram.setMnemonic(KeyEvent.VK_P);
    data.setMnemonic(KeyEvent.VK_D);
    closeTab.setMnemonic(KeyEvent.VK_C);
    closeTab.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, Event.CTRL_MASK));
    quit.setMnemonic(KeyEvent.VK_Q);
    quit.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, Event.CTRL_MASK));
    newCharacter.setMnemonic(KeyEvent.VK_N);
    newCharacter.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, Event.CTRL_MASK));
    menuCharacter.setMnemonic(KeyEvent.VK_H);
    saveCharacter.setMnemonic(KeyEvent.VK_S);
    saveCharacter.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, Event.CTRL_MASK));
    saveCharacterAs.setMnemonic(KeyEvent.VK_A);
    saveCharacterAs.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, Event.CTRL_MASK));
    loadCharacter.setMnemonic(KeyEvent.VK_O);
    loadCharacter.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, Event.CTRL_MASK));

    this.setJMenuBar(menuBar);
    this.addWindowListener(new WindowAdapter(){
      override def windowClosing(e:WindowEvent){
        Closer.close();
      }
    });

    levelEditor.addActionListener(new AbstractAction(){
      override def actionPerformed(event:ActionEvent){
        com.rafkind.paintown.Editor2.main(new Array[String](0));
      }
    });

    quit.addActionListener(new ActionListener() {
      override def actionPerformed(event:ActionEvent){
        Closer.close()
      }
    });

    clearCache.addActionListener( new ActionListener(){
      override def actionPerformed(event:ActionEvent){
        MaskedImage.clearCache();
      }
    });
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
