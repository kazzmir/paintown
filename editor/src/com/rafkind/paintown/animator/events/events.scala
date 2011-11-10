package com.rafkind.paintown.animator.events.scala

// import java.util._;
import java.awt.Graphics
import java.awt.Graphics2D
import java.awt.Color
import java.awt.Dimension
import java.awt.GridBagConstraints
import java.awt.event._;
import java.awt.image._
import javax.swing._;
import javax.swing.event._;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.BoundingBox;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.Lambda0;
import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.animator.NewAnimator
import org.swixml.SwingEngine;

import java.io.File
import com.rafkind.paintown.Data;
import com.rafkind.paintown.MaskedImage;

object Utils{
  def toScalaList[T](list:java.util.List[T]):List[T] = {
    var out:List[T] = List[T]()
    for (item <- scala.collection.JavaConversions.asScalaBuffer(list)){
      out = out :+ item
    }
    out
  }
}

class Attack(){
  val DEFAULT_FORCE_X = 1.7
  val DEFAULT_FORCE_Y = 4.4

  var x1:Int = 0
  var y1:Int = 0
  var x2:Int = 0
  var y2:Int = 0
  var damage:Int = 0
  var forceX:Double = DEFAULT_FORCE_X
  var forceY:Double = DEFAULT_FORCE_Y

  def copy() = {
    val attack = new Attack()
    attack.x1 = x1
    attack.y1 = y1
    attack.x2 = x2
    attack.y2 = y2
    attack.damage = damage
    attack.forceX = forceX
    attack.forceY = forceY
    attack
  }

  def isEmpty():Boolean = {
    x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0
  }
}

class AttackEvent extends AnimationEvent {
  var onDestroy = () => { }
  var attacks:List[Attack] = List[Attack]()

  private def addAttack(attack:Attack) = {
    attacks = attacks :+ attack
  }

  override def copy() = {
    val event = new AttackEvent()
    event.attacks = attacks.map((attack:Attack) => { attack.copy() })
    event
  }
    
  override def destroy() = {
    onDestroy()
  }

  def getDescription() = "Creates an attack box that is used to discover collissions between characters. x1, y1 specify the upper left hand corner of the box while x2, y2 specify the lower right hand corner. 'damage' specifies the amount of life this attack will take away and 'force' specifies the velocity that is given to the character being hit."

  def parseAttacks(token:Token):List[Attack] = {
    (parse(token) :: Utils.toScalaList(token.findTokens("box")).map(parse)).filter(x => ! x.isEmpty())
  }

  override def loadToken(token:Token){
    this.attacks = parseAttacks(token)
  }

  def parse(token:Token):Attack = {
    val attack = new Attack();

    val x1_token = token.findToken("x1");
    if (x1_token != null){
      attack.x1 = x1_token.readInt(0);
    }

    val y1_token = token.findToken("y1");
    if (y1_token != null){
      attack.y1 = y1_token.readInt(0);
    }

    val x2_token = token.findToken("x2");
    if (x2_token != null){
      attack.x2 = x2_token.readInt(0);
    }

    val y2_token = token.findToken("y2");
    if (y2_token != null){
      attack.y2 = y2_token.readInt(0);
    }

    val damage_token = token.findToken("damage");
    if (damage_token != null){
      attack.damage = damage_token.readInt(0);
    }

    val force_token = token.findToken("force");
    if (force_token != null){
      /*
      attack.force = force_token.readInt(0);
       */
      try{
        val x = force_token.readDouble(0);
        val y = force_token.readDouble(1);
        attack.forceX = x;
        attack.forceY = y;
      } catch {
        case fail:NoSuchElementException => {
        }
      }
    }

    attack
  }

  override def getToken():Token = {
    val temp = new Token();
    temp.addToken(new Token("attack"));
    for (attack <- attacks){
      if (attack.isEmpty()){
        // temp.addToken(new Token("box"));
      } else {
        val box = new Token();
        temp.addToken(box);
        box.addToken(new Token("box"));
        box.addToken(("x1" :: attack.x1.toString() :: List[String]()).toArray)
        box.addToken(("y1" :: attack.y1.toString() :: List[String]()).toArray)
        box.addToken(("x2" :: attack.x2.toString() :: List[String]()).toArray)
        box.addToken(("y2" :: attack.y2.toString() :: List[String]()).toArray)
        box.addToken(("force" :: attack.forceX.toString() :: attack.forceY.toString() :: List[String]()).toArray)
        box.addToken(("damage" :: attack.damage.toString() :: List[String]()).toArray)
        }
      }

      temp
    }

    override def getName():String = {
      getToken().toString()
    }

    override def getEditor(animation:Animation, area:DrawArea):JPanel = {
      if (attacks.isEmpty){
        attacks = List[Attack](new Attack())
        getEditor(animation, area, attacks.head)
      } else {
        getEditor(animation, area, attacks.head)
      }
    }

    override def interact(animation:Animation){
        if (attacks.isEmpty){
            animation.setAttack(new BoundingBox(0, 0, 0, 0));
        } else {
            val attack = attacks.head
            animation.setAttack(new BoundingBox(attack.x1, attack.y1, attack.x2, attack.y2))
        }
    }

    def getEditor(animation:Animation, area:DrawArea, attack:Attack):JPanel = {
        val engine = new SwingEngine("animator/eventattack.xml")
        // ((JPanel)engine.getRootComponent()).setSize(200,150);

        val x1spin = engine.find("x1").asInstanceOf[JSpinner]
        x1spin.setValue(new Integer(attack.x1));
        x1spin.addChangeListener(new ChangeListener(){
            def stateChanged(changeEvent:ChangeEvent){
                attack.x1 = x1spin.getValue().asInstanceOf[Integer].intValue()
                interact(animation);
                animation.forceRedraw();
            }
        });

        val y1spin = engine.find( "y1" ).asInstanceOf[JSpinner];
        y1spin.setValue(new Integer(attack.y1));
        y1spin.addChangeListener(new ChangeListener() {
          def stateChanged(changeEvent:ChangeEvent){
                attack.y1 = y1spin.getValue().asInstanceOf[Integer].intValue();
                interact(animation);
                animation.forceRedraw();
            }
        });
        
        val x2spin = engine.find("x2").asInstanceOf[JSpinner];
        x2spin.setValue(new Integer(attack.x2));
        x2spin.addChangeListener(new ChangeListener(){
            def stateChanged(changeEvent:ChangeEvent){
                attack.x2 = x2spin.getValue().asInstanceOf[Integer].intValue();
                interact(animation);
                animation.forceRedraw();
            }
        });

        val y2spin = engine.find("y2").asInstanceOf[JSpinner];
        y2spin.setValue(new Integer(attack.y2));
        y2spin.addChangeListener(new ChangeListener(){
            def stateChanged(changeEvent:ChangeEvent){
                attack.y2 = y2spin.getValue().asInstanceOf[Integer].intValue();
                interact(animation);
                animation.forceRedraw();
            }
        });

        val forcespinX = engine.find("forceX").asInstanceOf[JSpinner];
        forcespinX.setModel(new SpinnerNumberModel(attack.forceX, 0, 1000, 0.1));
        forcespinX.addChangeListener(new ChangeListener(){
            def stateChanged(changeEvent:ChangeEvent){
                attack.forceX = forcespinX.getValue().asInstanceOf[Double].doubleValue();
            }
        });
        
        val forcespinY = engine.find("forceY").asInstanceOf[JSpinner];
        forcespinY.setModel(new SpinnerNumberModel(attack.forceY, 0, 1000, 0.1));
        forcespinY.addChangeListener(new ChangeListener(){
          def stateChanged(changeEvent:ChangeEvent){
                attack.forceY = forcespinY.getValue().asInstanceOf[Double].doubleValue();
            }
        });

        val damagespin = engine.find("damage").asInstanceOf[JSpinner];
        damagespin.setValue(new Integer(attack.damage));
        damagespin.addChangeListener(new ChangeListener(){
            def stateChanged(changeEvent:ChangeEvent){
                attack.damage = damagespin.getValue().asInstanceOf[Integer].intValue();
            }
        });

        val toggle = engine.find("toggle").asInstanceOf[JButton];
        toggle.addActionListener(new AbstractAction(){
            var toggled:Boolean = false;
            val listener = new MouseInputAdapter(){
                override def mousePressed(e:MouseEvent){
                    attack.x1 = (e.getX() / area.getScale() - area.getCenterX() + animation.getWidth() / 2 - animation.getOffsetX()).toInt
                    attack.y1 = (e.getY() / area.getScale() - area.getCenterY() + animation.getHeight() - animation.getOffsetY()).toInt
                    x1spin.setValue(new Integer(attack.x1));
                    y1spin.setValue(new Integer(attack.y1));
                    interact(animation);
                    animation.forceRedraw();
                }

                override def mouseDragged(e:MouseEvent){
                    attack.x2 = (e.getX() / area.getScale() - area.getCenterX() + animation.getWidth() / 2 -
                    animation.getOffsetX()).toInt
                    attack.y2 = (e.getY() / area.getScale() - area.getCenterY() + animation.getHeight() - animation.getOffsetY()).toInt
                    x2spin.setValue(new Integer(attack.x2));
                    y2spin.setValue(new Integer(attack.y2));
                    interact(animation);
                    animation.forceRedraw();
                }
            };

            def actionPerformed(event:ActionEvent){
                if (toggled){
                    toggle.setText("Draw attack box");
                    area.enableMovement();
                    area.removeMouseListener(listener);
                    area.removeMouseMotionListener(listener);
                    onDestroy = () => {}
                } else {
                    toggle.setText("Stop drawing");
                    area.disableMovement();
                    area.addMouseListener(listener);
                    area.addMouseMotionListener(listener);
                    onDestroy = () => {
                      area.removeMouseListener(listener);
                      area.removeMouseMotionListener(listener);
                      area.enableMovement();
                    }
                }

                toggled = ! toggled;
            }
        });

        engine.getRootComponent().asInstanceOf[JPanel];
    }
}

class FrameEvent extends AnimationEvent {
    var frame:String = ""

    def loadToken(token:Token){
        frame = token.readString(0)
    }

    override def copy() = {
      val event = new FrameEvent()
      event.frame = frame
      event
    }

    def interact(animation:Animation){
        val path = Data.getDataPath() + "/" + animation.getBaseDirectory() + "/" + frame
        try{
            /*
               animation.setImage( MaskedImage.load( path ) );
               */
            if (animation.getMap() != null){
                animation.setImage(MaskedImage.load(path, animation.getMap()));
            } else {
                animation.setImage(MaskedImage.load(path));
            }
            animation.delay();
        } catch {
          case e:Exception => {
            e.printStackTrace();
            System.out.println("Could not load " + path);
          }
        }
    }
  
    def getDescription() = "Sets the current image to be shown. The frame event will wait for the 'delay' amount of time before moving on to the next event."

    def getName():String = {
        getToken().toString()
    }

    def getEditor(animation:Animation, area2:DrawArea):JPanel = {
        val engine = new SwingEngine("animator/eventframe.xml");
        // ((JPanel)engine.getRootComponent()).setSize(350,270);
        // JPanel canvas = (JPanel)engine.find("canvas");
        val canvas = engine.getRootComponent().asInstanceOf[JPanel]

        class drawArea extends JComponent {
            var img:BufferedImage  = null;
            override def paint(g:Graphics){
                g.setColor(new Color(0, 0, 0));
                // g.fillRect( 0, 0, 640, 480 );
                g.fillRect(1, 1, getWidth() - 1, getHeight() - 1);
                if (img != null){
                    // g.drawImage( img, 125 - (img.getTileWidth()/2), 100 - (img.getTileHeight()/2), null );
                    val g2d = g.asInstanceOf[Graphics2D];
                    val scale = Math.min((getWidth() - 5.0) / img.getTileWidth(), (getHeight() - 5.0) / img.getTileHeight());
                    g2d.scale(scale, scale);
                    // g.drawImage(img, (int)(getWidth() / 2 - (img.getTileWidth()*scale/2)), (int)(getHeight() / 2 - img.getTileHeight()*scale/2), null);
                    // g.drawImage(img, (int)(getWidth() / 2 - (img.getTileWidth()*scale/2)), (int)(getHeight() / 2 - img.getTileHeight()*scale/2), null);
                    g.drawImage(img, (getWidth() / 2 - (img.getWidth(null)*scale/2)).toInt, (getHeight() / 2 - img.getHeight(null)*scale/2).toInt, null);
                    // g.drawImage(img, (int) ((getWidth() / 2 - (img.getTileWidth()/2)) * scale), (int)((getHeight() / 2 - (img.getTileHeight()/2)) * scale), null);
                }
            }

            def setImage(i:BufferedImage){
                img = i;
            }
        };

        val area = new drawArea();

        /*
           area.setSize(350,200);
           area.setPreferredSize( new Dimension( 350,200 ) );
           */
        area.setPreferredSize(new Dimension(100,100));

        val constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1;
        constraints.weighty = 1;
        constraints.fill = GridBagConstraints.BOTH;
        constraints.anchor = GridBagConstraints.NORTHWEST;
        canvas.add(area, constraints);

        // canvas.add(area);

        val framebox = engine.find("frame").asInstanceOf[JComboBox]
        var index = 0;
        var count = -1;
        for (name <- NewAnimator.getFiles(animation.getBaseDirectory())){
            count += 1
            framebox.addItem(name);
            if (name.endsWith(frame)){
                index = count;
            }
        }

        framebox.addActionListener(new ActionListener(){
            override def actionPerformed(event:ActionEvent){
                frame = framebox.getSelectedItem().asInstanceOf[String]
                try{
                    area.setImage(MaskedImage.load(Data.getDataPath() + "/" + animation.getBaseDirectory() + "/" + frame));
                    area.repaint();
                } catch {
                  case e:Exception => {
                    System.out.println("Couldn't load file: " + frame);
                    e.printStackTrace();
                  }
                }
            }
        });

        framebox.setSelectedIndex(index);

        engine.getRootComponent().asInstanceOf[JPanel]
    }

    def setFrame(frame:String){
      this.frame = frame
    }

    def getToken():Token = {
        val temp = new Token("frame");
        temp.addToken(new Token("frame"));
        temp.addToken(new Token(frame));
        temp;
    }

    def destroy(){
    }
}

class MoveEvent extends AnimationEvent {
	var x:Int = 0
	var y:Int = 0
	var z:Int = 0

    override def copy() = {
      val event = new MoveEvent()
      event.x = x
      event.y = y
      event.z = z
      event
    }
	
	def loadToken(token:Token){
		x = token.readInt(0)
		y = token.readInt(1)
		z = token.readInt(2)
	}
	
	def interact(animation:Animation){
		animation.moveX(x);
		animation.moveY(y);
	}
	
	def getName():String = {
		getToken().toString();
	}
	
	def getEditor(animation:Animation, area:DrawArea):JPanel = {
		val engine = new SwingEngine("animator/eventmove.xml")
		engine.getRootComponent().asInstanceOf[JPanel].setSize(200,150)
		
		val xspin = engine.find("x").asInstanceOf[JSpinner];
		xspin.setValue(new Integer(x));
		xspin.addChangeListener(new ChangeListener(){
			def stateChanged(changeEvent:ChangeEvent){
				x = xspin.getValue().asInstanceOf[Integer].intValue()
			}
		});
		val yspin = engine.find("y").asInstanceOf[JSpinner];
		yspin.setValue(new Integer(y));
		yspin.addChangeListener(new ChangeListener(){
			def stateChanged(changeEvent:ChangeEvent){
				y = yspin.getValue().asInstanceOf[Integer].intValue();
			}
		});
		val zspin = engine.find("z").asInstanceOf[JSpinner];
		zspin.setValue(new Integer(z));
		zspin.addChangeListener(new ChangeListener(){
			def stateChanged(changeEvent:ChangeEvent){
				z = zspin.getValue().asInstanceOf[Integer].intValue()
			}
		});

		engine.getRootComponent().asInstanceOf[JPanel];
	}
	
    override def getToken():Token = {
		val temp = new Token("move")
		temp.addToken(new Token("move"))
		temp.addToken(new Token(x.toString()))
		temp.addToken(new Token(y.toString()))
		temp.addToken(new Token(z.toString()))
		temp
	}

    override def destroy(){
    }
    
    def getDescription() = "Moves the character by the given x, y, and z coordinates"
}

class HittableEvent extends AnimationEvent {
  var hit:Boolean = true

  override def copy() = {
    val event = new HittableEvent()
    event.hit = hit
    event
  }
  
  def destroy(){
  }
  
  def getName():String = getToken().toString()

  def getToken():Token = {
    val temp = new Token()
    temp.addToken(new Token("hittable"));
    temp.addToken(new Token(hit.toString()))
    temp
  }

  def loadToken(token:Token){
    hit = token.readBoolean(0)
  }
    
  def getDescription() = "Makes the character able to be attacked or not"
  
  def interact(animation:Animation){
  }

  def getEditor(animation:Animation, area:DrawArea):JPanel = {
    val engine = new SwingEngine("animator/event-hittable.xml");
    val change = engine.find("hit").asInstanceOf[JCheckBox]
    change.setSelected(hit)
    change.addChangeListener(new ChangeListener(){
      def stateChanged(changeEvent:ChangeEvent){
        hit = change.isSelected()
      }
    });

    engine.getRootComponent().asInstanceOf[JPanel];
  }
}

case class Defense(var x1:Int, var y1:Int, var x2:Int, var y2:Int){
}

class DefenseEvent extends AnimationEvent {

  var boxes:List[Defense] = List[Defense]()
  var onDestroy = () => null

  override def copy() = {
    val event = new DefenseEvent()
    event.boxes = boxes.map(box => box match {
      case Defense(x1, y1, x2, y2) => Defense(x1, y1, x2, y2)
    })
    event
  }

  def destroy(){
    onDestroy()
  }
    
  def getToken():Token = {
    val temp = new Token()
    temp.addToken(new Token("defense"));
    for (defense <- boxes.filter(!isEmpty(_))){
      val box = new Token();
      temp.addToken(box);
      box.addToken(new Token("box"));
      box.addToken(("x1" :: defense.x1.toString() :: List[String]()).toArray)
      box.addToken(("y1" :: defense.y1.toString() :: List[String]()).toArray)
      box.addToken(("x2" :: defense.x2.toString() :: List[String]()).toArray)
      box.addToken(("y2" :: defense.y2.toString() :: List[String]()).toArray)
    }
    temp
  }
    
  def getName():String = getToken().toString()
    
  def parse(token:Token):Defense = {
    val x1_token = token.findToken("x1");
    var x1 = 0
    var y1 = 0
    var x2 = 0
    var y2 = 0
    if (x1_token != null){
      x1 = x1_token.readInt(0);
    }

    val y1_token = token.findToken("y1");
    if (y1_token != null){
      y1 = y1_token.readInt(0);
    }

    val x2_token = token.findToken("x2");
    if (x2_token != null){
      x2 = x2_token.readInt(0);
    }

    val y2_token = token.findToken("y2");
    if (y2_token != null){
      y2 = y2_token.readInt(0);
    }

    Defense(x1, y1, x2, y2)
  }

  def isEmpty(defense:Defense):Boolean = {
    defense match {
      case Defense(x1, y1, x2, y2) => x1 != 0 || x2 != 0 || y1 != 0 || y2 != 0
    }
  }

  def loadToken(token:Token){
    this.boxes = (parse(token) :: Utils.toScalaList(token.findTokens("box")).map(parse)).filter(!isEmpty(_))
  }
    
  def getDescription() = "Not used yet"

  def interact(animation:Animation){
    if (boxes.isEmpty){
      animation.setDefense(new BoundingBox(0, 0, 0, 0));
    } else {
      boxes.head match {
        case Defense(x1, y1, x2, y2) => animation.setDefense(new BoundingBox(x1, y1, x2, y2))
      }
    }
  }

  def getEditor(animation:Animation, area:DrawArea):JPanel = {
    if (boxes.isEmpty){
      boxes = boxes :+ new Defense(0, 0, 0, 0)
    }

    getEditor(animation, area, boxes.head)
  }
 
  def getEditor(animation:Animation, area:DrawArea, defense:Defense):JPanel = {
    val engine = new SwingEngine("animator/event-defense.xml");
        // ((JPanel)engine.getRootComponent()).setSize(200,150);

    val x1spin = engine.find("x1").asInstanceOf[JSpinner]
    x1spin.setValue(new Integer(defense.x1));
    x1spin.addChangeListener(new ChangeListener(){
      def stateChanged(changeEvent:ChangeEvent){
        defense.x1 = x1spin.getValue().asInstanceOf[Integer].intValue()
        interact(animation);
        animation.forceRedraw();
      }
    });

    val y1spin = engine.find("y1").asInstanceOf[JSpinner]
    y1spin.setValue(new Integer(defense.y1));
    y1spin.addChangeListener( new ChangeListener() {
      def stateChanged(changeEvent:ChangeEvent){
        defense.y1 = y1spin.getValue().asInstanceOf[Integer].intValue()
        interact(animation);
        animation.forceRedraw();
      }
    });

    val x2spin = engine.find("x2").asInstanceOf[JSpinner]
    x2spin.setValue(new Integer(defense.x2));
    x2spin.addChangeListener(new ChangeListener(){
      def stateChanged(changeEvent:ChangeEvent){
        defense.x2 = x2spin.getValue().asInstanceOf[Integer].intValue()
        interact(animation);
        animation.forceRedraw();
      }
    });

    val y2spin = engine.find("y2").asInstanceOf[JSpinner]
    y2spin.setValue(new Integer(defense.y2));
    y2spin.addChangeListener(new ChangeListener(){
      def stateChanged(changeEvent:ChangeEvent){
        defense.y2 = y2spin.getValue().asInstanceOf[Integer].intValue()
        interact(animation);
        animation.forceRedraw();
      }
    });

    val toggle =  engine.find("toggle").asInstanceOf[JButton]
    toggle.addActionListener(new AbstractAction(){
      var toggled = false
      val listener = new MouseInputAdapter(){
        override def mousePressed(e:MouseEvent){
          defense.x1 = (e.getX() / area.getScale() - area.getCenterX() + animation.getWidth() / 2 - animation.getOffsetX()).toInt
          defense.y1 = (e.getY() / area.getScale() - area.getCenterY() + animation.getHeight() - animation.getOffsetY()).toInt
          x1spin.setValue(new Integer(defense.x1));
          y1spin.setValue(new Integer(defense.y1));
          interact(animation);
          animation.forceRedraw();
        }

        override def mouseDragged(e:MouseEvent){
          defense.x2 = (e.getX() / area.getScale() - area.getCenterX() + animation.getWidth() / 2 - animation.getOffsetX()).toInt
          defense.y2 = (e.getY() / area.getScale() - area.getCenterY() + animation.getHeight() - animation.getOffsetY()).toInt
          x2spin.setValue(new Integer(defense.x2));
          y2spin.setValue(new Integer(defense.y2));
          interact(animation);
          animation.forceRedraw();
        }
      };

      def actionPerformed(event:ActionEvent){
        if (toggled){
          toggle.setText("Draw defense box")
          area.enableMovement()
          area.removeMouseListener(listener)
          area.removeMouseMotionListener(listener)
          onDestroy = () => null
        } else {
          toggle.setText("Stop drawing")
          area.disableMovement()
          area.addMouseListener(listener)
          area.addMouseMotionListener(listener)
          onDestroy = () => {
            area.removeMouseListener(listener)
            area.removeMouseMotionListener(listener)
            area.enableMovement()
            null;
           }
        }
        toggled = ! toggled
      }
     });

      engine.getRootComponent().asInstanceOf[JPanel]
  }
}
