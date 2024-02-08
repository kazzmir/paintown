/*--
 $Id: InputEvent.java,v 1.1 2004/03/01 07:56:00 wolfpaulus Exp $

 Copyright (C) 2003-2007 Wolf Paulus.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions, and the disclaimer that follows
 these conditions in the documentation and/or other materials provided
 with the distribution.

 3. The end-user documentation included with the redistribution,
 if any, must include the following acknowledgment:
        "This product includes software developed by the
         SWIXML Project (http://www.swixml.org/)."
 Alternately, this acknowledgment may appear in the software itself,
 if and wherever such third-party acknowledgments normally appear.

 4. The name "Swixml" must not be used to endorse or promote products
 derived from this software without prior written permission. For
 written permission, please contact <info_AT_swixml_DOT_org>

 5. Products derived from this software may not be called "Swixml",
 nor may "Swixml" appear in their name, without prior written
 permission from the Swixml Project Management.

 THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED.  IN NO EVENT SHALL THE SWIXML PROJECT OR ITS
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
 ====================================================================

 This software consists of voluntary contributions made by many
 individuals on behalf of the Swixml Project and was originally
 created by Wolf Paulus <wolf_AT_swixml_DOT_org>. For more information
 on the Swixml Project, please see <http://www.swixml.org/>.
*/
package org.swixml.converters;

import java.awt.*;


public interface InputEvent {

  /**
   * The shift key modifier constant.
   * It is recommended that SHIFT_DOWN_MASK to be used instead.
   */
  public static final int SHIFT_MASK = Event.SHIFT_MASK;

  /**
   * The control key modifier constant.
   * It is recommended that CTRL_DOWN_MASK to be used instead.
   */
  public static final int CTRL_MASK = Event.CTRL_MASK;

  /**
   * The meta key modifier constant.
   * It is recommended that META_DOWN_MASK to be used instead.
   */
  public static final int META_MASK = Event.META_MASK;

  /**
   * The alt key modifier constant.
   * It is recommended that ALT_DOWN_MASK to be used instead.
   */
  public static final int ALT_MASK = Event.ALT_MASK;

  /**
   * The alt-graph key modifier constant.
   */
  public static final int ALT_GRAPH_MASK = 1 << 5;

  /**
   * The mouse button1 modifier constant.
   * It is recommended that BUTTON1_DOWN_MASK to be used instead.
   */
  public static final int BUTTON1_MASK = 1 << 4;

  /**
   * The mouse button2 modifier constant.
   * It is recommended that BUTTON2_DOWN_MASK to be used instead.
   */
  public static final int BUTTON2_MASK = Event.ALT_MASK;

  /**
   * The mouse button3 modifier constant.
   * It is recommended that BUTTON3_DOWN_MASK to be used instead.
   */
  public static final int BUTTON3_MASK = Event.META_MASK;

  /**
   * The SHIFT key extended modifier constant.
   * @since 1.4
   */
  public static final int SHIFT_DOWN_MASK = 1 << 6;

  /**
   * The CTRL key extended modifier constant.
   * @since 1.4
   */
  public static final int CTRL_DOWN_MASK = 1 << 7;

  /**
   * The META key extended modifier constant.
   * @since 1.4
   */
  public static final int META_DOWN_MASK = 1 << 8;

  /**
   * The ALT key extended modifier constant.
   * @since 1.4
   */
  public static final int ALT_DOWN_MASK = 1 << 9;

  /**
   * The mouse button1 extended modifier constant.
   * @since 1.4
   */
  public static final int BUTTON1_DOWN_MASK = 1 << 10;

  /**
   * The mouse button2 extended modifier constant.
   * @since 1.4
   */
  public static final int BUTTON2_DOWN_MASK = 1 << 11;

  /**
   * The mouse button3 extended modifier constant.
   * @since 1.4
   */
  public static final int BUTTON3_DOWN_MASK = 1 << 12;

  /**
   * The alt-graph key extended modifier constant.
   * @since 1.4
   */
  public static final int ALT_GRAPH_DOWN_MASK = 1 << 13;


  static final int JDK_1_3_MODIFIERS = SHIFT_DOWN_MASK - 1;

}
