/* 
   $Id: rst.st 768 2006-01-28 03:33:28Z marknodine $
   Copyright (C) 2002-2005 Freescale Semiconductor, Inc.
   Distributed under terms of the Perl license, which is the disjunction of
   the GNU General Public License (GPL) and the Artistic License.
*/

/*
  This is a drastically reduced states file meant only for highlighting ADL code.
*/

parsed_literal = 0;

adl_regfiles = "";
adl_regfiles_re = regexp(adl_regfiles);

adl_all_regs = "";
adl_all_regs_re = regexp(adl_all_regs);

sub custom_face (on, face) { 
}

sub rst_faces ()
{
  final_whitespace = "";
  in_markup = false;

  /* RGB -> PostScript color mapper function. */
  sub map_color (r, g, b)
  {
    return sprintf ("%f %f %f", r div 255.0, g div 255.0, b div 255.0);
  }

  /* Language special characters. */
  LANGUAGE_SPECIALS = /[_`*\|\\\\\n]/;

  sub language_print (str)
  {
    str = regsuball (str, /[_*`|\\\\]/, "\\$0");

    if (in_markup) {
      old_final_whitespace = final_whitespace;
      final_whitespace = regsub (str, /([ \n]*[^ \n]+)*([ \n]*)$/, "$2");
      str = regsub (str, /[ \n]*$/, "");
      str = concat(old_final_whitespace, str);
    }
    if (parsed_literal)
      str = regsuball (str, /\n/, "\n   ");

    /*print ("<");/* */
    print (str);
    /*print (">");/* */
  }

  sub header ()
  {
    if (parsed_literal)
      print (".. parsed-literal::\n\n   ");
    /* Nothing here. */
  }

  sub trailer ()
  {
    /* Nothing here. */
  }

  sub font (name)
  {
    print ("\0font{", name, "@", ptsize, "}");
  }

  sub color (name)
  {
    print ("\0color{", name, "}");
  }

  sub default ()
    {
      print ("\0font{default}");
      if (color)
        print ("\0color{default}");
    }

  sub bold (on)
  {
    if (on)
      {
        font (bold_font);
        if (color)
          color (bold_color);
      }
    else
    default ();
  }

  sub italic (on)
  {
    if (on)
      {
        font (italic_font);
        if (color)
          color (italic_color);
      }
    else
    default ();
  }

  sub bold_italic (on)
  {
    if (on)
      {
        font (bold_italic_font);
        if (color)
          color (bold_italic_color);
      }
    else
    default ();
  }

  sub start_markup (fontname) 
  {
    if (color)
      print("\\ `\\ ");
    else {
      markup_tag = strcmp(fontname, bold_font) ? "**" :
        strcmp(fontname, italic_font) ? "*" :
        strcmp(fontname, bold_italic_font) ? "***" : "";
      print ("\\ ", markup_tag, "\\ ");
    }
    final_whitespace = "";
    in_markup = true;
    LANGUAGE_SPECIALS = /./;
  }

  sub end_markup (class) 
  {
    if (color)
      print ("`:", class, ":");
    else
      print (markup_tag, "\\ ");
    in_markup = false;
    LANGUAGE_SPECIALS = /[_`*\|\\\\\n]/;
    language_print(final_whitespace);
  }
    
  sub comment_face (on)
  {
    if (on)
      start_markup(comment_face_font);
    else
      end_markup("comment");
      
  }

  sub function_name_face (on)
  {
    if (on) {
      start_markup(function_name_face_font);
    }
    else
      end_markup("function-name");
  }

  sub variable_name_face (on)
  {
    if (on)
      start_markup(variable_name_face_font);
    else
      end_markup("variable-name");
  }

  sub keyword_face (on)
  {
    if (on)
      start_markup(keyword_face_font);
    else
      end_markup("keyword");
  }

  sub reference_face (on)
  {
    if (on)
      start_markup(reference_face_font);
    else
      end_markup("reference-name");
  }

  sub string_face (on)
  {
    if (on)
      start_markup(string_face_font);
    else
      end_markup("string");
  }

  sub builtin_face (on)
  {
    if (on)
      start_markup(builtin_face_font);
    else
      end_markup("builtin");
  }

  sub type_face (on)
  {
    if (on)
      start_markup(type_face_font);
    else
      end_markup("type-name");
  }
  sub custom_face (on, class)
  {
    if (on)
      start_markup(font);
    else
      end_markup(class);
  }
}

/*
 * States definitions file for GNU Enscript.
 * Copyright (c) 1997-1998 Markku Rossi.
 * Author: Markku Rossi <mtr@iki.fi>
 *
 * The latest version of this file can be downloaded from URL:
 *
 *     http://www.iki.fi/~mtr/genscript/enscript.st
 */

/*
 * This file is part of GNU enscript.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * $Id: enscript.st,v 1.1.1.1 1999/04/20 23:26:45 wsanchez Exp $
 */

/*
 * Guildelines for writing new highlighting rules for the GNU Enscript.
 *
 * - all pretty-printing states should have a document comment like this:
 *
 *   /**
 *    * Name: c
 *    * Description: C programming language.
 *    * Author: Author Name <author@email.address>
 *    * ...
 *
 *   It is used by enscript's --help-pretty-print option to print
 *   description about supported pretty-printing modes.
 *
 * - BEGIN and END rules must call header() and trailer() subroutines:
 *
 *   state c
 *   {
 *     BEGIN {
 *	 header ();
 *     }
 *     END {
 *	 trailer ();
 *     }
 *
 * - each state and possible help-states should have LANGUAGE_SPECIALS rule:
 *
 *     LANGUAGE_SPECIALS {
 *	 language_print ($0);
 *     }
 *
 * - all printing should be done with the language_print() procedure
 *   instead of the print() primitive.
 *
 * These rules ensures that enscript's --help-pretty-print option works
 * and that the many2html utility script can convert all the
 * --pretty-print supported languages to HTML.
 */

/* This script needs at least this version of the States program. */
prereq ("1.5.0");

/*
 * Constants, fonts, etc.
 */

debug = "0";

/* Boolean values. */
true = 1;
false = 0;

font_spec = "Courier@10";

/* These components are resolved from <font_spec>. */
font = "";
ptsize = "";

/*
 * Highlight levels.  Possible values are:
 * - none	no highlighting
 * - light	light highlighting (comments and highlights)
 * - heavy	give all
 */
hl_level = "heavy";

/*
 * Colormodel.	Possible values are:
 * - blackwhite		no colors, just black and white
 * - emacs		similar to emacs' font lock highlighting
 */
colormodel = "blackwhite";

/*
 * Target language.  Possible values are:
 * - enscript		generate enscript special escapes
 * - html		generate HTML
 * - overstrike		generate overstrike (line printers, less)
 * - texinfo		generate Texinfo
 * - rtf                generate Rich Text Format (rtf - MS Word, WordPerfect)
 *                      This code can be suched into MS Word or PowerPoint
 *                      for a pretty version of the code
 * - rst                generate reStructuredText
 */
language = "enscript";

/*
 * How many input files we have.
 */
num_input_files = "1";
current_input_file = 0;

/*
 * Document title.
 */
document_title = "Enscript Output";


/*
 * Color definitions.
 */

cindex = 0;
rgb_values = list ();

sub define_color (name, r, g, b)
{
  rgb_values[cindex] = list (name, r, g, b);
  cindex = cindex + 1;
}

sub color_index (name)
{
  local i;

  for (i = 0; i < length (rgb_values); i = i + 1)
    if (strcmp (rgb_values[i][0], name) == 0)
      return i;

  return -1;
}

sub language_color (name)
{
  local idx;

  idx = color_index (name);
  if (idx < 0)
    panic ("unknown color `", name, "'");

  /*
   * The map_color() subroutine is language specific and defined in
   * *_faces() subroutine.
   */
  map_color (rgb_values[idx][1], rgb_values[idx][2], rgb_values[idx][3]);
}

/* RGB definitions for colors.	These are borrowed from X's rgb.txt file. */

define_color ("black",			0, 0, 0);
define_color ("blue",			0, 0, 255);
define_color ("cadet blue",		95, 158, 160);
define_color ("dark goldenrod",		184, 134, 11);
define_color ("dark olive green",	85, 107, 47);
define_color ("firebrick",		178, 34, 34);
define_color ("forest green",		34, 139, 34);
define_color ("orchid",			218, 112, 214);
define_color ("purple",			160, 32, 240);
define_color ("rosy brown",		188, 143, 143);

define_color ("DarkSeaGreen",		139, 179, 129);
define_color ("Goldenrod",		237, 218, 116);
define_color ("Aquamarine",		67, 183, 186);
define_color ("SeaGreen2",		100, 233, 134);
define_color ("Coral",			247, 101,  65);
define_color ("DarkSlateGray1",		154, 254, 255);


/*
 * General helpers.
 */

sub debug (msg)
{
  if (debug_level)
    print ("DEBUG: ", msg, "\n");
}

sub is_prefix (prefix, string)
{
  return strncmp (prefix, string, length (prefix)) == 0;
}

sub strchr (string, ch)
{
  local len = length (string), i;

  for (i = 0; i < len; i = i + 1)
    if (string[i] == ch)
      return i;

  return -1;
}

sub need_version (major, minor, beta)
{
  local r, v, i;

  regmatch (version, (/([0-9]+)\.([0-9]+)\.([0-9]+)/));
  v = list (int ($1), int ($2), int ($3));
  r = list (major, minor, beta);

  for (i = 0; i < 3; i = i + 1)
    if (v[i] > r[i])
      return 1;
    else if (v[i] < r[i])
      return 0;

  /* Exact match. */
  return 1;
}

/* Highlight types which match expression <re> from string <data>. */
sub highlight_types (data, re)
{
  local match_len;

  while (regmatch (data, re))
    {
      language_print ($B);
      type_face (true);
      language_print ($0);
      type_face (false);

      match_len = length ($B, $0);

      data = substring (data, match_len, length (data));
    }

  language_print (data);
}


sub define_faces ()
{
  if (strcmp (language, "rst") == 0)
    rst_faces ();
  else
    panic ("unknown output language `", language, "'");
}


/*
 * Initializations.
 */

start
{
  /* Set debug level. */
  debug_level = int (debug);

  /* Increment input file count. */
  current_input_file = current_input_file + 1;

  /* Resolve fonts. */
  idx = strchr (font_spec, '@');
  if (idx < 0)
    panic ("malformed font spec: `", font_spec, "'");

  font = substring (font_spec, 0, idx);
  ptsize = substring (font_spec, idx + 1, length (font_spec));

  debug (concat ("start: ", font, "@", ptsize));

  /* Construct bold, italic, etc. fonts for our current body font. */
  if (is_prefix ("AvantGarde", font))
    {
      bold_font = "AvantGarde-Demi";
      italic_font = "AvantGarde-BookOblique";
      bold_italic_font = "AvantGarde-DemiOblique";
    }
  else if (regmatch (font, /^Bookman|Souvenir/))
    {
      bold_font = concat ($0, "-Demi");
      italic_font = concat ($0, "-LightItalic");
      bold_italic_font = concat ($0, "-DemiItalic");
    }
  else if (regmatch (font, /^(.*)-Roman$/))
    {
      bold_font = concat ($1, "-Bold");
      italic_font = concat ($1, "-Italic");
      bold_italic_font = concat ($1, "-BoldItalic");
    }
  else if (regmatch (font, /^LucidaSans-/))
    {
      bold_font = concat (font, "Bold");
      italic_font = concat (font, "Oblique");
      bold_italic_font = concat (font, "BoldOblique");
    }
  else
    {
      bold_font = concat (font, "-Bold");
      italic_font = concat (font, "-Oblique");
      bold_italic_font = concat (font, "-BoldOblique");
    }

  /* Define output faces. */
  define_faces ();

  /* Select colormodel. */
  if (strcmp (colormodel, "blackwhite") == 0)
    {
      color = 0;
    }
  else if (strcmp (colormodel, "emacs") == 0)
    {
      color = 1;
      bold_color =			language_color ("black");
      italic_color =			language_color ("black");
      bold_italic_color =		language_color ("black");
      comment_face_color =		language_color ("firebrick");
      function_name_face_color =	language_color ("blue");
      variable_name_face_color =	language_color ("dark goldenrod");
      keyword_face_color =		language_color ("purple");
      reference_face_color =		language_color ("cadet blue");
      string_face_color =		language_color ("rosy brown");
      builtin_face_color =		language_color ("orchid");
      type_face_color =			language_color ("forest green");
    }
  else if (strcmp (colormodel, "ifh") == 0)
    {
      color = 1;
      bold_color =			language_color ("black");
      italic_color =			language_color ("black");
      bold_italic_color =		language_color ("black");
      comment_face_color =		language_color ("DarkSeaGreen");
      function_name_face_color =	language_color ("Coral");
      variable_name_face_color =	language_color ("dark goldenrod");
      keyword_face_color =		language_color ("SeaGreen2");
      reference_face_color =		language_color ("forest green");
      string_face_color =		language_color ("Goldenrod");
      reference_face_color =		language_color ("Aquamarine");
      builtin_face_color =		language_color ("purple");
      type_face_color =			language_color ("DarkSlateGray1");
    }
  else
    panic ("unknown color model `", colormodel, "'");

  /* Select highlight level. */
  if (strcmp (hl_level, "none") == 0)
    {
      comment_face_font =	font;
      function_name_face_font = font;
      variable_name_face_font = font;
      keyword_face_font =	font;
      reference_face_font =	font;
      string_face_font =	font;
      builtin_face_font =	font;
      type_face_font =		font;
    }
  else if (strcmp (hl_level, "light") == 0)
    {
      comment_face_font =	italic_font;
      function_name_face_font = bold_font;

      if (color)
        variable_name_face_font = font;
      else
        variable_name_face_font = bold_font;

      keyword_face_font =	bold_font;
      reference_face_font =	bold_font;
      string_face_font =	font;
      builtin_face_font =	font;
      type_face_font =		font;
    }
  else if (strcmp (hl_level, "heavy") == 0)
    {
      comment_face_font =	italic_font;
      function_name_face_font = bold_font;

      if (color)
        variable_name_face_font = font;
      else
        variable_name_face_font = bold_font;

      keyword_face_font =	bold_font;
      reference_face_font =	bold_font;
      string_face_font =	bold_font;
      builtin_face_font =	bold_font;
      type_face_font =		bold_font;
    }
  else
    panic ("unknown highlight level `", hl_level, "'");

  /* Resolve start state. */
  if (check_startrules ())
    debug ("startstate from startrules");
  if (check_namerules ())
    debug ("startstate from namerules");
}

namerules
{
  /\.(adl)$/				adl;
  /^.*$/				passthrough;
}

startrules
{
  /-\*- [Aa][Dd][Ll] -\*-/					adl;
}


/*
 * Helper subroutines and states.
 */

state match_balanced_block
{
  match_balanced_block_start {
    language_print ($0);
    match_balanced_block_count = match_balanced_block_count + 1;
  }

  match_balanced_block_end {
    match_balanced_block_count = match_balanced_block_count - 1;
    if (match_balanced_block_count == 0)
      return $0;

    language_print ($0);
  }

  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}

sub match_balanced_block (starter, ender)
{
  match_balanced_block_count = 1;
  match_balanced_block_start = starter;
  match_balanced_block_end = ender;
  return call (match_balanced_block);
}

state eat_one_line
{
  /.*\n/ {
    language_print ($0);
    return;
  }
}

/*
 * Pass all input through handling only output language specific headers
 * and LANGUAGE_SPECIALS.
 */
state passthrough
{
  BEGIN {
    debug ("passthrough");
    header ();
  }
  END {
    trailer ();
  }
  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}


/*
 * Describe all known highlight languages.
 */

state describe_me
{
  / \*$/ {
  }

  / \*\\\/.*/ {
    /* All done. */
    return;
  }

  / \* ?(.*)/ {
    print ($1);
  }
}

state describe_languages
{
  BEGIN {
    print ("Highlighting is supported for the following languages and file formats:\n");
  }
  END {
    print ("\n");
  }
  /^\/\*\*.*$/ {
    call (describe_me);
  }
  /[^\\\/]+/ {
    /* NOP */
  }
  /./ {
    /* NOP */
  }
}



/**
 * Name: c
 * Description: C programming language.
 * Author: Markku Rossi <mtr@iki.fi>
 */

c_keyword_re =
  /* Keywords, but not types, goto, and case.
     (build-re '(break continue default do else for if return sizeof
     switch while))
  */
  /\b(break|continue|d(efault|o)|else|for|if|return|s(izeof|witch)|while)\b/;

c_type_re =
  /* Types.
     (build-re '(auto char const double enum extern float int long
     register short signed static struct typedef union unsigned void
     volatile))
  */
  /\b(auto|c(har|onst)|double|e(num|xtern)|float|int|long|register\
      |s(hort|igned|t(atic|ruct))|typedef|un(ion|signed)|vo(id|latile))\b/;

state c_comment
{
  /\*\\\// {
    language_print ($0);
    return;
  }
  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}

state c_string
{
  /\\\\./ {
    language_print ($0);
  }
  /\"/ {
    language_print ($0);
    return;
  }
  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}

state c_ppline
{
  /* Comments within a pre-processor line. */
  /\/\*/ {
    comment_face (true);
    language_print ($0);
    call (c_comment);
    comment_face (false);
  }
  /* Include line. */
  /(include)([ \t]+)/ {
    reference_face (true);
    language_print ($1);
    reference_face (false);
    language_print ($2);
    call (c_ppline_include);
    return;
  }
  /* Define line. */
  /(define)([ \t]+)/ {
    reference_face (true);
    language_print ($1);
    reference_face (false);
    language_print ($2);
    call (c_ppline_define);
    return;
  }
  /* Simple directives:
     (build-re '(undef if ifdef ifndef endif elif else line error pragma))
  */
  /\b(e(l(if|se)|ndif|rror)|if(|def|ndef)|line|pragma|undef)\b/ {
    reference_face (true);
    language_print ($0);
    reference_face (false);
    call (c_ppline_tokensequence);
    return;
  }
  /* An unknown pre-processor directive. */
  /[a-zA-Z_][^ \t\n]*/ {
    reference_face (true);
    language_print ($0);
    reference_face (false);
    call (c_ppline_tokensequence);
    return;
  }
  /\n/ {
    language_print ($0);
    return;
  }
  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}

state c_ppline_include
{
  /\"/ {
    string_face (true);
    language_print ($0);
    call (c_string);
    string_face (false);
    call (c_ppline_comments_strings_chars);
    return;
  }
  /<[^<>]+>/ {
    string_face (true);
    language_print ($0);
    string_face (false);
    call (c_ppline_comments_strings_chars);
    return;
  }
  /[a-zA-Z_][a-zA-Z_0-9]*/ {
    variable_name_face (true);
    print ($0);
    variable_name_face (false);
    call (c_ppline_comments_strings_chars);
    return;
  }
  /\n/ {
    language_print ($0);
    return;
  }
  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}

state c_ppline_define
{
  /([a-zA-Z_][a-zA-Z_0-9]*)(\([^\)]*\))?/ {
    if (strcmp ($2, "") != 0)
      {
        function_name_face (true);
        language_print ($1);
        function_name_face (false);
        language_print ($2);
      }
    else
      {
        variable_name_face (true);
        language_print ($1);
        variable_name_face (false);
      }
    call (c_ppline_comments_strings_chars);
    return;
  }
  /\n/ {
    language_print ($0);
    return;
  }
  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}

state c_ppline_comments_strings_chars
{
  /* Comments. */
  /\/\*/ {
    comment_face (true);
    language_print ($0);
    call (c_comment);
    comment_face (false);
  }
  /* String constants. */
  /\"/ {
    string_face (true);
    language_print ($0);
    call (c_string);
    string_face (false);
  }
  /* Character constants. */
  /'.'|'\\\\.'/ {
    string_face (true);
    language_print ($0);
    string_face (false);
  }
  /\n/ {
    language_print ($0);
    return;
  }
  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}

state c_ppline_tokensequence
{
  /* Comments. */
  /\/\*/ {
    comment_face (true);
    language_print ($0);
    call (c_comment);
    comment_face (false);
  }
  /* String constants. */
  /\"/ {
    string_face (true);
    language_print ($0);
    call (c_string);
    string_face (false);
  }
  /* Character constants. */
  /'.'|'\\\\.'/ {
    string_face (true);
    language_print ($0);
    string_face (false);
  }
  /* defined() operators. */
  /(defined)(\()([^\)]+)(\))/ {
    reference_face (true);
    language_print ($1);
    reference_face (false);
    language_print ($2);

    variable_name_face (true);
    language_print ($3);
    variable_name_face (false);

    language_print ($4);
  }
  /* Variable references. */
  /\b[a-zA-Z_][a-zA-Z_0-9]*\b/ {
    variable_name_face (true);
    language_print ($0);
    variable_name_face (false);
  }
  /\n/ {
    language_print ($0);
    return;
  }
  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}

state c
{
  BEGIN {
    if (need_version (1, 5, 1))
      c_function_name_re
        = /^([A-Za-z][a-zA-Z0-9_\* ]+)([ \*])([a-zA-Z_][a-zA-Z_0-9]*)([ \t]*\()/;
    else
      c_function_name_re = 0;

    debug ("c");
    header ();
  }
  END {
    trailer ();
  }

  /* Comments. */
  /\/\*/ {
    comment_face (true);
    language_print ($0);
    call (c_comment);
    comment_face (false);
  }
  /\/\// {
    comment_face (true);
    language_print ($0);
    call (eat_one_line);
    comment_face (false);
  }

  /* String constants. */
  /\"/ {
    string_face (true);
    language_print ($0);
    call (c_string);
    string_face (false);
  }

  /* Pre-processor lines. */
  /^#/ {
    language_print ($0);
    call (c_ppline);
  }

  /* Character constants. */
  /'.'|'\\\\.'/ {
    string_face (true);
    language_print ($0);
    string_face (false);
  }

  /* Keywords. */
  c_keyword_re {
    keyword_face (true);
    language_print ($0);
    keyword_face (false);
  }

  /* Types. */
  c_type_re {
    type_face (true);
    language_print ($0);
    type_face (false);
  }

  /* Labels and case tags.  Emacs accepts also bare numbers. */
  /([a-zA-Z0-9_]+)(:)/ {
    if (strcmp ($1, "default") == 0)
      {
        /* `default' is a keyword. */
        keyword_face (true);
        language_print ($1);
        keyword_face (false);
      }
    else
      {
        reference_face (true);
        language_print ($1);
        reference_face (false);
      }

    language_print ($2);
  }

  /* Goto, case and the target. */
  /\<(goto|case)\>([ \t]+)(-?[A-Za-z_][A-Za-z_0-9]*)?/ {
    keyword_face (true);
    language_print ($1);
    keyword_face (false);

    language_print ($2);

    if (length ($3) > 0)
      {
        reference_face (true);
        language_print ($3);
        reference_face (false);
      }
  }

  /*
   * Function definitions, but only if you code with the one and only
   * usable indentation style (GNU).
   */
  /^([a-zA-Z_][a-zA-Z_0-9]*)([ \t]*\()/ {
    function_name_face (true);
    language_print ($1);
    function_name_face (false);

    language_print ($2);
  }

  /* Function definitions and prototypes for other (loser) coding styles. */
  c_function_name_re {
    garbage = $1;
    middle_garbage = $2;
    function_name = $3;
    tail_garbage = $4;

    highlight_types (garbage, c_type_re);

    language_print (middle_garbage);

    function_name_face (true);
    language_print (function_name);
    function_name_face (false);

    language_print (tail_garbage);
  }

  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}



/**
 * Name: adl
 * Description: ADL programming language.  Basically just C++ with a few minor additions.
 * Author: Markku Rossi <mtr@iki.fi>
 */

adl_keyword_re =
  /* Keywords, but not types, goto, and case.
     (build-re '(asm break catch continue default delete do else for if
     new operator overload return sizeof switch this throw try while))
  */
  /\b(func|asm|break|c(atch|ontinue)|d(e(fault|lete)|o)|else|var|for|if|new\
      |o(perator|verload)|return|s(izeof|witch)|t(h(is|row)|ry)|while)\b/;

adl_type_re =
  /* Types.
     (build-re '(auto bool char class complex const double enum extern
     float friend inline int long private protected public register
     short signed static struct template typedef typename union unsigned virtual
     void volatile))
  */
  /\b(bits|auto|bool|c(har|lass|o(mplex|nst))|double|e(num|xtern)|f(loat|riend)\
      |in(line|t)|long|p(r(ivate|otected)|ublic)|register\
      |s(hort|igned|t(atic|ruct))|t(emplate|ype(def|name))|un(ion|signed)\
      |v(irtual|o(id|latile)))\b/;

state adl
{
  BEGIN {
    if (need_version (1, 5, 1))
      adl_function_name_re
        = /^([A-Za-z][a-zA-Z0-9_\* ]+)([ \*])([a-zA-Z_][a-zA-Z_0-9:~]*)([ \t]*\()/;
    else
      adl_function_name_re = 0;

    debug ("adl");
    header ();
  }
  END {
    trailer ();
  }

  /* Comments. */
  /\/\*/ {
    comment_face (true);
    language_print ($0);
    call (c_comment);
    comment_face (false);
  }
  /\/\// {
    comment_face (true);
    language_print ($0);
    call (eat_one_line);
    comment_face (false);
  }

  /* String constants. */
  /\"/ {
    string_face (true);
    language_print ($0);
    call (c_string);
    string_face (false);
  }

  /* Pre-processor lines. */
  /^#/ {
    language_print ($0);
    call (c_ppline);
  }

  /* Character constants. */
  /'.'|'\\\\.'/ {
    string_face (true);
    language_print ($0);
    string_face (false);
  }

  /* Keywords. */
  adl_keyword_re {
    keyword_face (true);
    language_print ($0);
    keyword_face (false);
  }

  /* Types. */
  adl_type_re {
    type_face (true);
    language_print ($0);
    type_face (false);
  }

  /* Remove false labels. */
  /[a-zA-Z0-9_]+::/ {
    language_print ($0);
  }

  /* Labels and case tags.  Emacs accepts also bare numbers. */
  /([a-zA-Z0-9_]+)(:)/ {
    if (strcmp ($1, "default") == 0)
      {
        /* `default' is a keyword. */
        keyword_face (true);
        language_print ($1);
        keyword_face (false);
      }
    else if (strcmp ($1, "public") == 0
             || strcmp ($1, "private") == 0
             || strcmp ($1, "protected") == 0)
      {
        /* These use the `type' face. */
        type_face (true);
        language_print ($1);
        type_face (false);
      }
    else
      {
        reference_face (true);
        language_print ($1);
        reference_face (false);
      }

    language_print ($2);
  }

  /* Goto and its target. */
  /\<(goto|case)\>([ \t]+)(-?[A-Za-z_][A-Za-z_0-9]*)?/ {
    keyword_face (true);
    language_print ($1);
    keyword_face (false);

    language_print ($2);

    if (length ($3) > 0)
      {
        reference_face (true);
        language_print ($3);
        reference_face (false);
      }
  }

  /*
   * Function definitions, but only if you code with the one and only
   * usable indentation style (GNU).
   */
  /^([a-zA-Z_][a-zA-Z_0-9:~]*)([ \t]*\()/ {
    function_name_face (true);
    language_print ($1);
    function_name_face (false);

    language_print ($2);
  }

  /*
    I experimented with using UTF-8 directly in this file, but ran into problems
    b/c prest thought != was a nbsp due to the last byte being 0xa0.  So, I
    switched to using text substitutions, which prest translates into MathML
    symbols.
  */

  /*
    Convert != to equals-with-slash symbol:  U+2260, UTF-8 0xe289a0
  */
  /!=/ {
    print("|ne|");
  }

  /*
    Convert <= to logical less-than or equal symbol:  U+2264 UTF-8 0xe289a4
  */
  /<=/ {
    /*print("≤");*/
    print("|le|");
  }

  /*
    Convert >= to logical greater-than or equal symbol:  U+2265 UTF-8 0xe289a5
  */
  />=/ {
    /*print("≥");*/
    print("|ge|");
  }

  /*
    Convert && to logical-and symbol:  U+2227 UTF-8 0xe288a7
  */
  /&&/ {
    /*print("∧");*/
    print("|la|");
  }

  /*
    Convert || to logical-or symbol:  U+2228 UTF-8 0xe288a8
  */
  /\|\|/ {
    /*print("∨");*/
    print("|lo|");
  }

  /*
    Convert ^ operator to circle-plus symbol:  U+2295 or UTF-8 0xe28a95
   */
  /\^/ {
    /*print("⊕");*/
    print("|xor|");
  }

  /*
    Register-file accesses have their index converted to a subscript.
   */
  adl_regfiles_re {
    variable_name_face(true);
    language_print($1);
    variable_name_face(false);
    custom_face(true,"sub");
    language_print($2);
    custom_face(false,"sub");
   }

  /*
    High-light all known registers/register-files.
  */
  adl_all_regs_re {
    variable_name_face(true);
    language_print($0);
    variable_name_face(false);
  }

  /*
    Bits values have their size value converted to a subscript.
  */
  /\b(bits)[ \t]*<([^>]+)>/ {
    type_face (true);
    language_print($1);
    type_face (false);
    custom_face(true,"sub");
    language_print($2);
    custom_face(false,"sub");
   }

  /\b(zero)[ \t]*\(([^)]+)\)/ {
    keyword_face (true);
    language_print("0");
    keyword_face (false);
    custom_face(true,"sub");
    language_print($2);
    custom_face(false,"sub");
   }

  
  /* Function definitions and prototypes for other (loser) coding styles. */
  adl_function_name_re {
    garbage = $1;
    middle_garbage = $2;
    function_name = $3;
    tail_garbage = $4;

    highlight_types (garbage, adl_type_re);

    language_print (middle_garbage);

    function_name_face (true);
    language_print (function_name);
    function_name_face (false);

    language_print (tail_garbage);
  }

  LANGUAGE_SPECIALS {
    language_print ($0);
  }
}



      /*
        Local variables:
        mode: c
        End:
      */
