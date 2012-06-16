package com.rafkind.paintown.animator

import javax.swing._
import java.awt.event._
import java.awt.Color
import java.io.File
import javax.swing.event.ChangeListener
import javax.swing.event.ChangeEvent
import javax.swing.event.DocumentListener
import javax.swing.event.DocumentEvent
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
        } catch {
          case e:Exception => {
            area.setOverlayImage(null)
          }
        }
      } else {
        area.setOverlayImage(null)
      }
      area.repaint()
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

    filename.addActionListener(new AbstractAction(){
      def actionPerformed(event:ActionEvent){
        update(filename.getText())
      }
    })

    val flipx = context.find("overlay:flip-x").asInstanceOf[JCheckBox]
    val flipy = context.find("overlay:flip-y").asInstanceOf[JCheckBox]

    flipx.addActionListener(new AbstractAction(){
      def actionPerformed(event:ActionEvent){
        area.setOverlayFlipX(flipx.isSelected());
        area.repaint();
      }
    })

    flipy.addActionListener(new AbstractAction(){
      def actionPerformed(event:ActionEvent){
        area.setOverlayFlipY(flipy.isSelected());
        area.repaint();
      }
    })

    val offsetx = context.find("overlay:x").asInstanceOf[JSpinner]
    val offsety = context.find("overlay:y").asInstanceOf[JSpinner]

    offsetx.setValue(new Integer(area.getOverlayImageOffsetY()))
    offsetx.addChangeListener(new ChangeListener(){
      def stateChanged(event:ChangeEvent){
        area.setOverlayImageOffsetX(offsetx.getValue().asInstanceOf[Integer].intValue())
        area.repaint()
      }
    })

    offsety.setValue(new Integer(area.getOverlayImageOffsetY()))
    offsety.addChangeListener(new ChangeListener(){
      def stateChanged(event:ChangeEvent){
        area.setOverlayImageOffsetY(offsety.getValue().asInstanceOf[Integer].intValue())
        area.repaint()
      }
    })

    val alphaText = context.find("overlay:alpha-text").asInstanceOf[JLabel]
    val alpha = context.find("overlay:alpha").asInstanceOf[JSlider]

    alpha.setValue((area.getOverlayImageAlpha() * alpha.getMaximum()).toInt);
    alphaText.setText("Transparency " + area.getOverlayImageAlpha());
    alpha.addChangeListener(new ChangeListener(){
      def stateChanged(change:ChangeEvent){
        area.setOverlayImageAlpha(alpha.getValue().asInstanceOf[Double].doubleValue() /
        alpha.getMaximum().asInstanceOf[Double].doubleValue())
        alphaText.setText("Transparency " + area.getOverlayImageAlpha())
        area.repaint();
      }
    })

    val front = context.find("overlay:front").asInstanceOf[JRadioButton];
    val back = context.find("overlay:back").asInstanceOf[JRadioButton];
    front.setActionCommand("front");
    back.setActionCommand("back");

    val change = new AbstractAction(){
      def actionPerformed(event:ActionEvent){
        if (event.getActionCommand().equals("front")){
          area.setOverlayImageFront();
        } else {
          area.setOverlayImageBehind();
        }
        area.repaint();
      }
    }

    front.addActionListener(change);
    back.addActionListener(change);

    context.find("overlay-image").asInstanceOf[JPanel]
  }

}
