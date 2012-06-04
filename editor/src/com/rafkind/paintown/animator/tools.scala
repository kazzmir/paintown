package com.rafkind.paintown.animator

import javax.swing._
import java.awt.Color
import javax.swing.event.ChangeListener
import javax.swing.event.ChangeEvent
import com.rafkind.paintown.Undo
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

}
