package com.rafkind.paintown;

import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.imageio.*;

public class MaskedImage extends BufferedImage {

	public MaskedImage( int w, int h ){
		super( w, h, BufferedImage.TYPE_INT_ARGB );
	}

	public static MaskedImage load( String s ) throws IOException {
		BufferedImage temp = ImageIO.read( new File( s ) );
		MaskedImage image = new MaskedImage( temp.getWidth(), temp.getHeight() );
		for ( int x = 0; x < temp.getWidth(); x++ ){
			for ( int y = 0; y < temp.getHeight(); y++ ){
				int pixel = temp.getRGB( x, y );
				if ( (pixel & 0x00ffffff) == 0x00ff00ff ){
					/* convert masking color into an alpha channel that is translucent */
					pixel = 0x00ffffff;
				}
				image.setRGB( x, y, pixel );
			}
		}
		return image;
	}
}
