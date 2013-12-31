package com.rafkind.platformer

import java.awt._
import java.awt.image._
import java.io._
import javax.imageio._

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.Token
import com.rafkind.paintown.MaskedImage

class Frame{
    var image:Int = 0
    var alpha:Int = 0 
    var hflip:Boolean = false
    var vflip:Boolean = false 
    var time:Int = 0

    def toToken():Token = {
        val frame = new Token()
        frame.addToken(new Token(frame, "frame"))
        frame.addToken(Array("image", String.valueOf(image)))
        frame.addToken(Array("hflip", String.valueOf(if(hflip) 1 else 0)))
        frame.addToken(Array("vflip", String.valueOf(if(vflip) 1 else 0)))
        frame.addToken(Array("time", String.valueOf(time)))
        frame
    }
}

class Animation{
    var name:String = _
    var basedir:File = _
    var images:Map[Int, String] = _
    var frames:Vector[Frame] = _
    var loop:Int = 0
    
    var scale:Double = 2
    
    def render(g:Graphics2D, x:Int, y:Int, width:Int, height:Int) = {
        g.scale(scale, scale)
        
        // Render
    }
    
    def loadImage(s:String):Image = {
        try{
            MaskedImage.load( MapEditor.getDataPath( s ).getPath() )
        } catch {
            case ex:IOException => throw new LoadException( "Could not load " + s )
        }
    }

    def toToken():Token = {
        val animation = new Token()
        animation.addToken(new Token(animation, "animation"))
        animation.addToken(Array("id", name))
        animation.addToken(Array("basedir", basedir.getPath()))
        images.foreach {
            case (key, image) => animation.addToken(Array("image", String.valueOf(key), image))
        }
        frames.foreach {
            case (frame) => animation.addToken(frame.toToken())
        }

        animation
    }
}
