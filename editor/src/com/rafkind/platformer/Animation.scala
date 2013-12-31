package com.rafkind.platformer

import java.awt._
import java.awt.image._
import java.io._
import javax.imageio._

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.Token
import com.rafkind.paintown.MaskedImage

class ImageHolder(i:Image, f:File){
    val image:Image = i
    val file:File = f
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        g.drawImage(image, x, y, null)
    }
}

class Frame{
    var image:Int = -1
    var alpha:Int = -1
    var hflip:Boolean = false
    var vflip:Boolean = false 
    var time:Int = 0

    def toToken():Token = {
        val frame = new Token()
        frame.addToken(new Token(frame, "frame"))
        frame.addToken(Array("image", String.valueOf(image)))
        if (alpha != -1){
            frame.addToken(Array("alpha", String.valueOf(alpha)))
        }
        frame.addToken(Array("hflip", String.valueOf(if(hflip) 1 else 0)))
        frame.addToken(Array("vflip", String.valueOf(if(vflip) 1 else 0)))
        frame.addToken(Array("time", String.valueOf(time)))
        frame
    }
}

class Animation{
    var name:String = _
    var basedir:File = _
    var images = scala.collection.mutable.HashMap.empty[Int, ImageHolder]
    var frames = scala.collection.mutable.ArrayBuffer.empty[Frame]
    var loop:Int = 0
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        // Render
        if (frames.size > 0){
            // Get first
            var frame = frames(0)
            if (frame.image != -1){
                images(frame.image).render(g, x, y)
            }
        }
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
            case (key, image) => animation.addToken(Array("image", String.valueOf(key), image.file.getPath()))
        }
        frames.foreach {
            case (frame) => animation.addToken(frame.toToken())
        }
        animation.addToken(Array("loop", String.valueOf(loop)))

        animation
    }
}
