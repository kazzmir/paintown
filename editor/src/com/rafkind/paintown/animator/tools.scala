package com.rafkind.paintown.animator

import javax.swing._
import java.awt.event._
import java.awt.Color
import java.io.File
import javax.swing.event.ChangeListener
import javax.swing.event.ChangeEvent
import com.rafkind.paintown.Undo
import com.rafkind.paintown.MaskedImage
import org.swixml.SwingEngine;

object Tools{
  def makeBackgroundTool(character:AnimatedObject, area:DrawArea):JPanel = {
    var panel = new JPanel()
    val color = new JColorChooser(area.backgroundColor());
    color.setPreviewPanel(new JPanel());
    panel.add(color);
    color.getSelectionModel().addChangeListener(new ChangeListener(){
      val self = this
      def stateChanged(change:ChangeEvent){
        val old = character.getDrawProperties().getBackgroundColor()
        character.getDrawProperties().setBackgroundColor(color.getSelectionModel().getSelectedColor())
        area.repaint()

        Undo.addUndo("Set color to " + old, () => {
          color.getSelectionModel().removeChangeListener(self)
          character.getDrawProperties().setBackgroundColor(old)
          color.getSelectionModel().setSelectedColor(old)
          area.repaint()
          color.getSelectionModel().addChangeListener(self)
        })
      }
    });

    character.getDrawProperties().addListener(new DrawPropertiesListener(){
      override def updateBackgroundColor(newColor:Color){
        color.setColor(newColor);
        area.repaint();
      }
    });

    panel
  }

  def makeGridTool(area:DrawArea):JPanel = {
    val context = new SwingEngine("animator/animation-tools.xml")
    val guide = context.find("guide").asInstanceOf[JSlider];
    guide.setValue(area.getGuideSize());
    guide.addChangeListener(new ChangeListener(){
      def stateChanged(change:ChangeEvent){
        area.setGuideSize(guide.getValue());
        area.repaint();
      }
    });

    context.find("grid").asInstanceOf[JPanel]
  }

  def makeOverlayImageTool(parent:JPanel, area:DrawArea):JPanel = {
    val context = new SwingEngine("animator/animation-tools.xml")
    val enableButton = context.find("overlay:enable").asInstanceOf[JCheckBox]
    var lastFile:String = ""

    def update(path:String){
      lastFile = path
      if (enableButton.isSelected()){
        try{
          area.setOverlayImage(MaskedImage.load(path))
        }
      } else {
        area.setOverlayImage(null)
      }
    }
    
    enableButton.addActionListener(new AbstractAction(){
      def actionPerformed(event:ActionEvent){
        if (enableButton.isSelected()){
          enableButton.setText("Enabled")
        } else {
          enableButton.setText("Disabled")
        }

        update(lastFile)
      }
    })

    val filename = context.find("overlay:file").asInstanceOf[JTextField]
    val choose = context.find("overlay:choose").asInstanceOf[JButton]
    choose.addActionListener(new AbstractAction(){
      def actionPerformed(event:ActionEvent){
        val file = new JFileChooser()
        val value = file.showOpenDialog(parent)
        if (value == JFileChooser.APPROVE_OPTION){
          val selected = file.getSelectedFile()
          filename.setText(selected.getPath())
          update(selected.getPath())
        }
      }
    })

    context.find("overlay-image").asInstanceOf[JPanel]
  }

}
