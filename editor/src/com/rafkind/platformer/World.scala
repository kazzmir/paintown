package com.rafkind.platformer

import java.awt._
import java.awt.image._
import java.io._
import javax.imageio._

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.Token
import com.rafkind.paintown.MaskedImage

class World(loadfile:File){
    
    if (loadfile != null){
        load(loadfile)
    }
    
    var offsetX:Int = 15
    var offsetY:Int = 15
    
    var name:String = "No name assigned."
    var width:Int = 640
    var height:Int = 480
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
    
    //! Animations
    var animations = scala.collection.mutable.HashMap.empty[String, Animation]
    
    //! Tilesets
    var tilesets = scala.collection.mutable.ArrayBuffer.empty[TileSet]
    tilesets += new TileSet()
    
    def render(g:Graphics2D, x:Int, y:Int, width:Int, height:Int) = {
        g.scale(scale, scale)
        
        // Entire map
        g.setColor( new Color( 230, 230, 250 ) )
        g.fillRect(offsetX, offsetY, this.width, this.height)
        
        // Tilesets
        tilesets.foreach{
            case (tileset) => tileset.render(g, offsetX, offsetY)
        }
        
        // Viewport resolution
        g.setColor( new Color( 0, 0, 255 ) )
        g.drawRect(offsetX, offsetY, this.resolutionX, this.resolutionY)
    }

    def load(f:File) = {
        val reader = new TokenReader( f )
        val head = reader.nextToken()
        if ( ! head.getName().equals( "world" ) ){
            throw new LoadException( "Starting token is not 'world'" )
        }

        System.out.println( "Loaded " + f )
    }

    def loadImage(s:String):Image = {
        try{
            MaskedImage.load( MapEditor.getDataPath( s ).getPath() )
        } catch {
            case ex:IOException => throw new LoadException( "Could not load " + s )
        }
    }

    def getSize():Dimension = {
        val dimension = new Dimension()
        dimension.setSize((width * scale) + (offsetX*2), (height * scale) + (offsetY*2))
        dimension
    }

    def toToken():Token = {
        val world = new Token()
        world.addToken( new Token( world, "world" ) )
        

        world
    }
}
