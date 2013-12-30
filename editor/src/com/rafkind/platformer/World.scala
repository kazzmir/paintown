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

object World{
    
    var name:String = "No name assigned."
    var width:Int = 0
    var scale:Double = 0
    var path:File = null
    
    object Panel{
        def Panel(n:String, i:Image) = {
            val name = n
            val image = i
            
        }
    }
    
    def World(path:File) = {
        this.path = path
        load( path )
    }

    def getPath():File = {
        path
    }

    def setPath(path:File) = {
        this.path = path
    }

    def render(g:Graphics2D, x:Int, y:Int, width:Int, height:Int) = {
        // g.clearRect( 0, 0, (int) getWidth(), (int) getHeight() );
        g.scale( getScale(), getScale() );
        g.setColor( new Color( 255, 0, 0 ) );
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
        new Dimension( 32, 32 )
    }

    def toToken():Token = {
        val world = new Token()
        world.addToken( new Token( world, "world" ) )
        

        world
    }
    
    def getScale():Double = {
        scale
    }

    def setScale(s:Double) = {
        scale = s
    }
}
