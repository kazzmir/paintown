package com.rafkind.paintown;

import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.imageio.*;
import java.util.HashMap;

public class MaskedImage extends BufferedImage {

	private static HashMap images = new HashMap();

	public MaskedImage( int w, int h ){
		super( w, h, BufferedImage.TYPE_INT_ARGB );
	}

	public MaskedImage( BufferedImage image ){
		this( image.getWidth(), image.getHeight() );
		for ( int x = 0; x < image.getWidth(); x++ ){
			for ( int y = 0; y < image.getHeight(); y++ ){
				int pixel = image.getRGB( x, y );
				this.setRGB( x, y, pixel );
			}
		}
	}

	public static void clearCache(){
		synchronized( images ){
			images.clear();
		}
	}

	public static MaskedImage load(String s) throws IOException {
		synchronized (images){
			if (images.get(s) != null){
				return (MaskedImage) images.get(s);
			}
		}
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
		synchronized (images){
			images.put(s, image);
		}
		return image;
	}

	public static MaskedImage load( String s, HashMap remap ) throws IOException {
		String full = s + "\n" + String.valueOf( remap.hashCode() );
		synchronized (images){
			if (images.containsKey(full)){
				return (MaskedImage) images.get(full);
			}
		}

		MaskedImage image = new MaskedImage(load(s));
		for (int x = 0; x < image.getWidth(); x++){
			for (int y = 0; y < image.getHeight(); y++){
				int pixel = image.getRGB( x, y );
				if ( remap.containsKey( new Integer( pixel ) ) ){
					Integer mapped = (Integer) remap.get( new Integer( pixel ) );
					image.setRGB( x, y, mapped.intValue() );
				}
			}
		}

		synchronized (images){
			images.put(full, image);
		}

		return image;
	}
}
