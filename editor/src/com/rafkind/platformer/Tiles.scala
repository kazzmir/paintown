package com.rafkind.platformer

import java.awt._
import java.awt.image._
import java.io._
import javax.imageio._

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.Token
import com.rafkind.paintown.MaskedImage

class Tile{
    var animation:Animation = _
    
    var row:Int = 0
    var column:Int = 0
    
    var x:Int = 0
    var y:Int = 0
    var width:Int = 0
    var height:Int = 0
    
    def toToken():Token = {
        val tile = new Token()
        tile.addToken(new Token(tile, "tile"))
        
        tile
    }
}

class TileSet{
    
    var scrollX:Double = 0
    var scrollY:Double = 0
    
    var width:Int = 640
    var height:Int = 480
    
    var tileWidth:Int = 16
    var tileHeight:Int = 16
    
    var tiles = scala.collection.mutable.ArrayBuffer.empty[Tile]
    
    var renderGrid:Boolean = true
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        if (renderGrid){
            var currentY = y
            while (currentY < (height + y)){
                var currentX = x
                while (currentX < (width + x)){
                    g.setColor( new Color( 255, 255, 255 ) )
                    g.drawRect(currentX, currentY, tileWidth, tileHeight)
                    currentX += tileWidth
                }
                currentY += tileHeight
            }
        }
    }

    def toToken():Token = {
        val tileset = new Token()
        tileset.addToken(new Token(tileset, "tileset"))
        tileset.addToken(Array("tile-size", String.valueOf(tileWidth), String.valueOf(tileHeight)))
        
        tiles.foreach {
            case (tile) => tileset.addToken(tile.toToken())
        }
        
        tileset
    }
}
