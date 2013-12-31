package com.rafkind.platformer

import java.awt._
import java.awt.image._
import java.io._
import javax.imageio._

import java.util.Iterator
import java.util.List
import java.util.ArrayList
import java.util.HashMap
import java.util.Map
import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.Token
import com.rafkind.paintown.MaskedImage

class World(loadfile:File){
    
    if (loadfile != null){
        load(loadfile)
    }
    var name:String = "No name assigned."
    var width:Int = 320
    var height:Int = 240
    var scale:Double = 2
    var _path = loadfile
    
    def this() = this(null)
    def path = _path
    def path_= (value:File):Unit = _path = value
    
    //! Resolution of map (upscaled/downscaled according to paintowns screensize)
    var resolutionX:Int = 320
    var resolutionY:Int = 240
    
    //! mechanics
    var gravityX:Double = 0
    var gravityY:Double = 0
    var acceleration:Double = 0
    
    object Panel{
        def Panel(n:String, i:Image) = {
            val name = n
            val image = i
            
        }
    }

    def render(g:Graphics2D, x:Int, y:Int, width:Int, height:Int) = {
        g.scale( getScale(), getScale() )
        g.setColor( new Color( 230, 230, 250 ) )
        g.fillRect(5, 5, getWidth(), getHeight())
    }

    def load(f:File) = {
        val reader = new TokenReader( f );
        val head = reader.nextToken();
        if ( ! head.getName().equals( "world" ) ){
            throw new LoadException( "Starting token is not 'world'" );
        }

        System.out.println( "Loaded " + f );
    }

    def loadImage(s:String):Image = {
        try{
            MaskedImage.load( MapEditor.getDataPath( s ).getPath() )
        } catch {
            case ex:IOException => throw new LoadException( "Could not load " + s )
        }
    }

    def getSize():Dimension = {
        new Dimension( width, height )
    }

    def toToken():Token = {
        val world = new Token()
        world.addToken( new Token( world, "world" ) )
        

        world
    }
    
    def getWidth():Int = {
        width
    }
    
    def getHeight():Int = {
        height
    }
    
    def getScale():Double = {
        scale
    }

    def setScale(s:Double) = {
        scale = s
    }
}
