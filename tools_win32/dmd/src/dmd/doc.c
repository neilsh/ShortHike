
// Copyright (c) 1999-2005 by Digital Mars
// All Rights Reserved
// written by Walter Bright
// www.digitalmars.com
// License for redistribution is by either the Artistic License
// in artistic.txt, or the GNU General Public License in gnu.txt.
// See the included readme.txt for details.

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>

#ifdef IN_GCC
#include "mem.h"
#else
#if _WIN32
#include "..\root\mem.h"
#elif linux
#include "../root/mem.h"
#else
#error "fix this"
#endif
#endif

#include "root.h"

#include "mars.h"
#include "dsymbol.h"
#include "macro.h"
#include "template.h"
#include "lexer.h"
#include "aggregate.h"
#include "declaration.h"
#include "enum.h"
#include "id.h"
#include "module.h"
#include "scope.h"
#include "hdrgen.h"
#include "doc.h"
#include "mtype.h"

struct Section
{
    unsigned char *name;
    unsigned namelen;

    unsigned char *body;
    unsigned bodylen;

    int nooutput;

    virtual void write(DocComment *dc, Scope *sc, Dsymbol *s, OutBuffer *buf);
};

struct ParamSection : Section
{
    void write(DocComment *dc, Scope *sc, Dsymbol *s, OutBuffer *buf);
};

struct MacroSection : Section
{
    void write(DocComment *dc, Scope *sc, Dsymbol *s, OutBuffer *buf);
};

struct DocComment
{
    Array sections;		// Section*[]

    Section *summary;
    Section *copyright;
    Section *macros;
    Macro **pmacrotable;

    DocComment();

    static DocComment *parse(Scope *sc, Dsymbol *s, unsigned char *comment);
    static void parseMacros(Macro **pmacrotable, unsigned char *m, unsigned mlen);

    void parseSections(unsigned char *comment);
    void writeSections(Scope *sc, Dsymbol *s, OutBuffer *buf);
};


int cmp(char *stringz, void *s, size_t slen);
int icmp(char *stringz, void *s, size_t slen);
int isDitto(unsigned char *comment);
unsigned char *skipwhitespace(unsigned char *p);
unsigned skiptoident(OutBuffer *buf, unsigned i);
unsigned skippastident(OutBuffer *buf, unsigned i);
void highlightText(Scope *sc, Dsymbol *s, OutBuffer *buf, unsigned offset);
void highlightCode(Scope *sc, Dsymbol *s, OutBuffer *buf, unsigned offset);
void highlightCode2(Scope *sc, Dsymbol *s, OutBuffer *buf, unsigned offset);
Argument *isFunctionParameter(Dsymbol *s, unsigned char *p, unsigned len);

static unsigned char ddoc_default[] = "\
DDOC =	<html><head>\n\
	<META http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n\
	<title>$(TITLE)</title>\n\
	</head><body>\n\
	<h1>$(TITLE)</h1>\n\
	$(BODY)\n\
	<hr>$(SMALL Page generated by $(LINK2 http://www.digitalmars.com/d/ddoc.html, Ddoc). $(COPYRIGHT))\n\
	</body></html>\n\
\n\
B =	<b>$0</b>\n\
I =	<i>$0</i>\n\
U =	<u>$0</u>\n\
P =	<p>$0</p>\n\
DL =	<dl>$0</dl>\n\
DT =	<dt>$0</dt>\n\
DD =	<dd>$0</dd>\n\
TABLE =	<table>$0</table>\n\
TR =	<tr>$0</tr>\n\
TH =	<th>$0</th>\n\
TD =	<td>$0</td>\n\
OL =	<ol>$0</ol>\n\
UL =	<ul>$0</ul>\n\
LI =	<li>$0</li>\n\
BIG =	<big>$0</big>\n\
SMALL =	<small>$0</small>\n\
BR =	<br>\n\
LINK =	<a href=\"$0\">$0</a>\n\
LINK2 =	<a href=\"$1\">$+</a>\n\
\n\
RED =	<font color=red>$0</font>\n\
BLUE =	<font color=blue>$0</font>\n\
GREEN =	<font color=green>$0</font>\n\
YELLOW =<font color=yellow>$0</font>\n\
BLACK =	<font color=black>$0</font>\n\
WHITE =	<font color=white>$0</font>\n\
\n\
D_CODE = <pre class=\"d_code\">$0</pre>\n\
D_COMMENT = $(GREEN $0)\n\
D_STRING  = $(RED $0)\n\
D_KEYWORD = $(BLUE $0)\n\
D_PSYMBOL = $(U $0)\n\
D_PARAM	  = $(I $0)\n\
\n\
DDOC_DECL      = $(DT $(BIG $0))\n\
DDOC_DECL_DD   = $(DD $0)\n\
DDOC_DITTO     = $(BR)$0\n\
DDOC_SECTIONS  = $0\n\
DDOC_SUMMARY   = $0$(BR)$(BR)\n\
DDOC_DESCRIPTION = $0$(BR)$(BR)\n\
DDOC_AUTHORS   = $(B Authors:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_BUGS      = $(RED BUGS:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_COPYRIGHT = $(B Copyright:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_DATE      = $(B Date:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_DEPRECATED = $(RED Deprecated:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_EXAMPLES  = $(B Examples:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_HISTORY   = $(B History:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_LICENSE   = $(B License:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_RETURNS   = $(B Returns:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_SEE_ALSO  = $(B See Also:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_STANDARDS = $(B Standards:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_THROWS    = $(B Throws:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_VERSION   = $(B Version:)$(BR)\n$0$(BR)$(BR)\n\
DDOC_SECTION_H = $(B $0)$(BR)\n\
DDOC_SECTION   = $0$(BR)$(BR)\n\
DDOC_MEMBERS   = $(DL $0)\n\
DDOC_MODULE_MEMBERS = $(DDOC_MEMBERS $0)\n\
DDOC_CLASS_MEMBERS  = $(DDOC_MEMBERS $0)\n\
DDOC_STRUCT_MEMBERS = $(DDOC_MEMBERS $0)\n\
DDOC_ENUM_MEMBERS   = $(DDOC_MEMBERS $0)\n\
DDOC_TEMPLATE_MEMBERS = $(DDOC_MEMBERS $0)\n\
DDOC_PARAMS    = $(B Params:)$(BR)\n$(TABLE $0)$(BR)\n\
DDOC_PARAM_ROW = $(TR $0)\n\
DDOC_PARAM_ID  = $(TD $0)\n\
DDOC_PARAM_DESC = $(TD $0)\n\
DDOC_BLANKLINE	= $(BR)$(BR)\n\
\n\
DDOC_PSYMBOL	= $(U $0)\n\
DDOC_KEYWORD	= $(B $0)\n\
DDOC_PARAM	= $(I $0)\n\
";

static char ddoc_decl_s[] = "$(DDOC_DECL ";
static char ddoc_decl_e[] = ")\n";

static char ddoc_decl_dd_s[] = "$(DDOC_DECL_DD ";
static char ddoc_decl_dd_e[] = ")\n";


/****************************************************
 */

void Module::gendocfile()
{
    static OutBuffer mbuf;
    static int mbuf_done;

    OutBuffer buf;

    //printf("Module::gendocfile()\n");

    if (!mbuf_done)		// if not already read the ddoc files
    {	mbuf_done = 1;

	// Use our internal default
	mbuf.write(ddoc_default, sizeof(ddoc_default) - 1);

	// Override with DDOCFILE specified in the sc.ini file
	char *p = getenv("DDOCFILE");
	if (p)
	    global.params.ddocfiles->shift(p);

	// Override with the ddoc macro files from the command line
	for (int i = 0; i < global.params.ddocfiles->dim; i++)
	{
	    FileName f((char *)global.params.ddocfiles->data[i], 0);
	    File file(&f);
	    file.readv();
	    // BUG: convert file contents to UTF-8 before use

	    //printf("file: '%.*s'\n", file.len, file.buffer);
	    mbuf.write(file.buffer, file.len);
	}
    }
    DocComment::parseMacros(&macrotable, mbuf.data, mbuf.offset);

    Scope *sc = Scope::createGlobal(this);	// create root scope
    sc->docbuf = &buf;

    DocComment *dc = DocComment::parse(sc, this, comment);
    dc->pmacrotable = &macrotable;

    // Generate predefined macros

    // Set the title to be the name of the module
    {	char *p = toPrettyChars();
	Macro::define(&macrotable, (unsigned char *)"TITLE", 5, (unsigned char *)p, strlen(p));
    }

    time_t t;
    time(&t);
    char *p = ctime(&t);
    p = mem.strdup(p);
    Macro::define(&macrotable, (unsigned char *)"DATETIME", 8, (unsigned char *)p, strlen(p));
    Macro::define(&macrotable, (unsigned char *)"YEAR", 4, (unsigned char *)p + 20, 4);


    if (dc->copyright)
    {
	dc->copyright->nooutput = 1;
	Macro::define(&macrotable, (unsigned char *)"COPYRIGHT", 9, dc->copyright->body, dc->copyright->bodylen);
    }

    buf.printf("<!-- Generated by Ddoc from %s -->\n", srcfile->toChars());

    if (isDocFile)
    {
	size_t commentlen = strlen((char *)comment);
	if (dc->macros)
	{
	    commentlen = dc->macros->name - comment;
	    dc->macros->write(dc, sc, this, sc->docbuf);
	}
	sc->docbuf->write(comment, commentlen);
	highlightText(NULL, this, sc->docbuf, 0);
    }
    else
    {

	dc->writeSections(sc, this, sc->docbuf);
	emitMemberComments(sc);
    }

    //printf("BODY= '%.*s'\n", buf.offset, buf.data);
    Macro::define(&macrotable, (unsigned char *)"BODY", 4, buf.data, buf.offset);

    OutBuffer buf2;
    buf2.writestring("$(DDOC)\n");
    unsigned end = buf2.offset;
    macrotable->expand(&buf2, 0, &end, NULL, 0);

#if 1
    /* Remove all the escape sequences from buf2,
     * and make CR-LF the newline.
     */
    {
	buf.setsize(0);
	buf.reserve(buf2.offset);
	unsigned char *p = buf2.data;
	for (unsigned j = 0; j < buf2.offset; j++)
	{
	    unsigned char c = p[j];
	    if (c == 0xFF && j + 1 < buf2.offset)
	    {
		j++;
		continue;
	    }
	    if (c == '\n')
		buf.writeByte('\r');
	    else if (c == '\r')
	    {
		buf.writestring("\r\n");
		if (j + 1 < buf2.offset && p[j + 1] == '\n')
		{
		    j++;
		}
		continue;
	    }
	    buf.writeByte(c);
	}
    }

    // Transfer image to file
    assert(docfile);
    docfile->setbuffer(buf.data, buf.offset);
    docfile->ref = 1;
    docfile->writev();
#else
    /* Remove all the escape sequences from buf2
     */
    {	unsigned i = 0;
	unsigned char *p = buf2.data;
	for (unsigned j = 0; j < buf2.offset; j++)
	{
	    if (p[j] == 0xFF && j + 1 < buf2.offset)
	    {
		j++;
		continue;
	    }
	    p[i] = p[j];
	    i++;
	}
	buf2.setsize(i);
    }

    // Transfer image to file
    docfile->setbuffer(buf2.data, buf2.offset);
    docfile->ref = 1;
    docfile->writev();
#endif
}

/******************************* emitComment **********************************/

/*
 * Emit doc comment to documentation file
 */

void Dsymbol::emitDitto(Scope *sc)
{
    OutBuffer *buf = sc->docbuf;
    unsigned o;
    OutBuffer b;

    b.writestring("$(DDOC_DITTO ");
    o = b.offset;
    toDocBuffer(&b);
    highlightCode(sc, this, &b, o);
    b.writeByte(')');
    buf->spread(sc->lastoffset, b.offset);
    memcpy(buf->data + sc->lastoffset, b.data, b.offset);
    sc->lastoffset += b.offset;
}

void ScopeDsymbol::emitMemberComments(Scope *sc)
{
    //printf("ScopeDsymbol::emitMemberComments()\n");
    OutBuffer *buf = sc->docbuf;

    if (members)
    {	char *m = "$(DDOC_MEMBERS \n";

	if (isModule())
	    m = "$(DDOC_MODULE_MEMBERS \n";
	else if (isClassDeclaration())
	    m = "$(DDOC_CLASS_MEMBERS \n";
	else if (isStructDeclaration())
	    m = "$(DDOC_STRUCT_MEMBERS \n";
	else if (isEnumDeclaration())
	    m = "$(DDOC_ENUM_MEMBERS \n";
	else if (isTemplateDeclaration())
	    m = "$(DDOC_TEMPLATE_MEMBERS \n";

	// BUG: if no members are actually printed, we should not emit DDOC_MEMBERS
	buf->writestring(m);
	sc = sc->push(this);
	for (int i = 0; i < members->dim; i++)
	{
	    Dsymbol *s = (Dsymbol *)members->data[i];
	    //printf("\ts = '%s'\n", s->toChars());
	    s->emitComment(sc);
	}
	sc->pop();
	buf->writestring(")\n");
    }
}

void emitProtection(OutBuffer *buf, PROT prot)
{
    char *p;

    switch (prot)
    {
	case PROTpackage:	p = "package";	 break;
	case PROTprotected:	p = "protected"; break;
	case PROTexport:	p = "export";	 break;
	default:		p = NULL;	 break;
    }
    if (p)
	buf->printf("%s ", p);
}

void Dsymbol::emitComment(Scope *sc)		   { }
void InvariantDeclaration::emitComment(Scope *sc)  { }
void DtorDeclaration::emitComment(Scope *sc)	   { }
void StaticCtorDeclaration::emitComment(Scope *sc) { }
void StaticDtorDeclaration::emitComment(Scope *sc) { }
void ClassInfoDeclaration::emitComment(Scope *sc)  { }
void ModuleInfoDeclaration::emitComment(Scope *sc) { }
void TypeInfoDeclaration::emitComment(Scope *sc)   { }


void Declaration::emitComment(Scope *sc)
{
    //printf("Declaration::emitComment(%p '%s'), comment = '%s'\n", this, toChars(), comment);
    //printf("type = %p\n", type);

    if (protection == PROTprivate || !ident ||
	(!type && !isCtorDeclaration() && !isAliasDeclaration()))
	return;
    if (!comment)
	return;

    OutBuffer *buf = sc->docbuf;
    DocComment *dc = DocComment::parse(sc, this, comment);
    unsigned o;

    if (!dc)
    {
	emitDitto(sc);
	return;
    }
    dc->pmacrotable = &sc->module->macrotable;

    buf->writestring(ddoc_decl_s);
	o = buf->offset;
	toDocBuffer(buf);
	highlightCode(sc, this, buf, o);
	sc->lastoffset = buf->offset;
    buf->writestring(ddoc_decl_e);

    buf->writestring(ddoc_decl_dd_s);
    dc->writeSections(sc, this, buf);
    buf->writestring(ddoc_decl_dd_e);
}

void AggregateDeclaration::emitComment(Scope *sc)
{
    //printf("AggregateDeclaration::emitComment() '%s'\n", toChars());
    if (prot() == PROTprivate)
	return;
    if (!comment)
	return;

    OutBuffer *buf = sc->docbuf;
    DocComment *dc = DocComment::parse(sc, this, comment);

    if (!dc)
    {
	emitDitto(sc);
	return;
    }
    dc->pmacrotable = &sc->module->macrotable;

    buf->writestring(ddoc_decl_s);
    toDocBuffer(buf);
    sc->lastoffset = buf->offset;
    buf->writestring(ddoc_decl_e);

    buf->writestring(ddoc_decl_dd_s);
    dc->writeSections(sc, this, buf);
    emitMemberComments(sc);
    buf->writestring(ddoc_decl_dd_e);
}

void TemplateDeclaration::emitComment(Scope *sc)
{
    //printf("TemplateDeclaration::emitComment() '%s', kind = %s\n", toChars(), kind());
    if (prot() == PROTprivate)
	return;
    if (!comment)
	return;

    OutBuffer *buf = sc->docbuf;
    DocComment *dc = DocComment::parse(sc, this, comment);
    unsigned o;

    if (!dc)
    {
	emitDitto(sc);
	return;
    }
    dc->pmacrotable = &sc->module->macrotable;

    ScopeDsymbol *ss = this;

    if (onemember)
    {
	ss = onemember->isAggregateDeclaration();
	if (!ss)
	    ss = this;
    }

    buf->writestring(ddoc_decl_s);
	o = buf->offset;
	ss->toDocBuffer(buf);
	//highlightCode(sc, this, buf, o);
	sc->lastoffset = buf->offset;
    buf->writestring(ddoc_decl_e);

    buf->writestring(ddoc_decl_dd_s);
    dc->writeSections(sc, this, buf);
    ss->emitMemberComments(sc);
    buf->writestring(ddoc_decl_dd_e);
}

void EnumDeclaration::emitComment(Scope *sc)
{
    if (prot() == PROTprivate)
	return;
    if (!comment)
    {	if (isAnonymous() && members)
	{
	    for (int i = 0; i < members->dim; i++)
	    {
		Dsymbol *s = (Dsymbol *)members->data[i];
		s->emitComment(sc);
	    }
	}
	return;
    }
    if (isAnonymous())
	return;

    OutBuffer *buf = sc->docbuf;
    DocComment *dc = DocComment::parse(sc, this, comment);

    if (!dc)
    {
	emitDitto(sc);
	return;
    }
    dc->pmacrotable = &sc->module->macrotable;

    buf->writestring(ddoc_decl_s);
	toDocBuffer(buf);
	sc->lastoffset = buf->offset;
    buf->writestring(ddoc_decl_e);

    buf->writestring(ddoc_decl_dd_s);
    dc->writeSections(sc, this, buf);
    emitMemberComments(sc);
    buf->writestring(ddoc_decl_dd_e);
}

void EnumMember::emitComment(Scope *sc)
{
    //printf("EnumMember::emitComment(%p '%s'), comment = '%s'\n", this, toChars(), comment);
    if (prot() == PROTprivate)
	return;
    if (!comment)
	return;

    OutBuffer *buf = sc->docbuf;
    DocComment *dc = DocComment::parse(sc, this, comment);
    unsigned o;

    if (!dc)
    {
	emitDitto(sc);
	return;
    }
    dc->pmacrotable = &sc->module->macrotable;

    buf->writestring(ddoc_decl_s);
	o = buf->offset;
	toDocBuffer(buf);
	highlightCode(sc, this, buf, o);
	sc->lastoffset = buf->offset;
    buf->writestring(ddoc_decl_e);

    buf->writestring(ddoc_decl_dd_s);
    dc->writeSections(sc, this, buf);
    buf->writestring(ddoc_decl_dd_e);
}

/******************************* toDocBuffer **********************************/

void Dsymbol::toDocBuffer(OutBuffer *buf)
{
    HdrGenState hgs;

    toCBuffer(buf, &hgs);
}

void Declaration::toDocBuffer(OutBuffer *buf)
{
    //printf("Declaration::toDocbuffer() %s\n", toChars());
    if (ident)
    {
	if (isDeprecated())
	    buf->writestring("deprecated ");

	emitProtection(buf, protection);
	if (isAbstract())
	    buf->writestring("abstract ");
	if (isStatic())
	    buf->writestring("static ");
	if (isConst())
	    buf->writestring("const ");
	if (isFinal())
	    buf->writestring("final ");
	if (isSynchronized())
	    buf->writestring("synchronized ");
	if (type)
	{   HdrGenState hgs;
	    hgs.ddoc = 1;
	    type->toCBuffer(buf, ident, &hgs);
	}
	else
	    buf->writestring(ident->toChars());
	buf->writestring(";\n");
    }
}


void AliasDeclaration::toDocBuffer(OutBuffer *buf)
{
    //printf("AliasDeclaration::toDocbuffer() %s\n", toChars());
    if (ident)
    {
	if (isDeprecated())
	    buf->writestring("deprecated ");

	emitProtection(buf, protection);
	buf->writestring("alias ");
	buf->writestring(toChars());
	buf->writestring(";\n");
    }
}


void TypedefDeclaration::toDocBuffer(OutBuffer *buf)
{
    if (ident)
    {
	if (isDeprecated())
	    buf->writestring("deprecated ");

	emitProtection(buf, protection);
	buf->writestring("typedef ");
	buf->writestring(toChars());
	buf->writestring(";\n");
    }
}


void CtorDeclaration::toDocBuffer(OutBuffer *buf)
{
    HdrGenState hgs;

    buf->writestring("this");
    Argument::argsToCBuffer(buf, &hgs, arguments, varargs);
    buf->writestring(";\n");
}


void AggregateDeclaration::toDocBuffer(OutBuffer *buf)
{
    if (ident)
    {
#if 0
	emitProtection(buf, protection);
#endif
	buf->printf("%s $(DDOC_PSYMBOL %s)", kind(), toChars());
	buf->writestring(";\n");
    }
}

void ClassDeclaration::toDocBuffer(OutBuffer *buf)
{
    //printf("ClassDeclaration::toDocbuffer() %s\n", toChars());
    if (ident)
    {
#if 0
	emitProtection(buf, protection);
#endif
	TemplateDeclaration *td;

	if (parent &&
	    (td = parent->isTemplateDeclaration()) != NULL &&
	    td->onemember == this)
	{   unsigned o = buf->offset;
	    td->toDocBuffer(buf);
	    highlightCode(NULL, this, buf, o);
	}
	else
	{
	    buf->printf("%s $(DDOC_PSYMBOL %s)", kind(), toChars());
	}
	int any = 0;
	for (int i = 0; i < baseclasses.dim; i++)
	{   BaseClass *bc = (BaseClass *)baseclasses.data[i];

	    if (bc->protection == PROTprivate)
		continue;
	    if (bc->base && bc->base->ident == Id::Object)
		continue;

	    if (any)
		buf->writestring(", ");
	    else
	    {	buf->writestring(": ");
		any = 1;
	    }
	    emitProtection(buf, bc->protection);
	    if (bc->base)
	    {
		buf->writestring(bc->base->toPrettyChars());
	    }
	    else
	    {
		HdrGenState hgs;
		bc->type->toCBuffer(buf, NULL, &hgs);
	    }
	}
	buf->writestring(";\n");
    }
}


void EnumDeclaration::toDocBuffer(OutBuffer *buf)
{
    if (ident)
    {
	buf->printf("%s $(DDOC_PSYMBOL %s)", kind(), toChars());
	buf->writestring(";\n");
    }
}

void EnumMember::toDocBuffer(OutBuffer *buf)
{
    if (ident)
    {
	buf->writestring(toChars());
    }
}


/********************************* DocComment *********************************/

DocComment::DocComment()
{
    memset(this, 0, sizeof(DocComment));
}

DocComment *DocComment::parse(Scope *sc, Dsymbol *s, unsigned char *comment)
{   unsigned idlen;

    if (sc->lastdc && isDitto(comment))
	return NULL;

    DocComment *dc = new DocComment();
    if (!comment)
	return dc;

    dc->parseSections(comment);

    for (int i = 0; i < dc->sections.dim; i++)
    {	Section *s = (Section *)dc->sections.data[i];

	if (icmp("copyright", s->name, s->namelen) == 0)
	{
	    dc->copyright = s;
	}
	if (icmp("macros", s->name, s->namelen) == 0)
	{
	    dc->macros = s;
	}
    }

    sc->lastdc = dc;
    return dc;
}

/*****************************************
 * Parse next paragraph out of *pcomment.
 * Update *pcomment to point past paragraph.
 * Returns NULL if no more paragraphs.
 * If paragraph ends in 'identifier:',
 * then (*pcomment)[0 .. idlen] is the identifier.
 */

void DocComment::parseSections(unsigned char *comment)
{   unsigned char *p;
    unsigned char *pstart;
    unsigned char *pend;
    unsigned char *q;
    unsigned char *idstart;
    unsigned idlen;

    unsigned char *name = NULL;
    unsigned namelen = 0;

    p = comment;
    while (*p)
    {
	p = skipwhitespace(p);
	pstart = p;

	/* Find end of section, which is ended by one of:
	 *	'identifier:'
	 *	'\0'
	 */
	idlen = 0;
	while (1)
	{
	    if (isalpha(*p) || *p == '_')
	    {
		q = p + 1;
		while (isalnum(*q) || *q == '_')
		    q++;
		if (*q == ':')	// identifier: ends it
		{   idlen = q - p;
		    idstart = p;
		    for (pend = p; pend > pstart; pend--)
		    {	if (pend[-1] == '\n')
			    break;
		    }
		    p = q + 1;
		    break;
		}
	    }
	    while (1)
	    {
		if (!*p)
		{   pend = p;
		    goto L1;
		}
		if (*p == '\n')
		{   p++;
		    if (*p == '\n' && !summary && !namelen)
		    {
			pend = p;
			p++;
			goto L1;
		    }
		    break;
		}
		p++;
	    }
	    p = skipwhitespace(p);
	}
      L1:

	if (namelen || pstart < pend)
	{
	    Section *s;
	    if (icmp("Params", name, namelen) == 0)
		s = new ParamSection();
	    else if (icmp("Macros", name, namelen) == 0)
		s = new MacroSection();
	    else
		s = new Section();
	    s->name = name;
	    s->namelen = namelen;
	    s->body = pstart;
	    s->bodylen = pend - pstart;
	    s->nooutput = 0;

	    //printf("Section: '%.*s' = '%.*s'\n", s->namelen, s->name, s->bodylen, s->body);

	    sections.push(s);

	    if (!summary && !namelen)
		summary = s;
	}

	if (idlen)
	{   name = idstart;
	    namelen = idlen;
	}
	else
	{   name = NULL;
	    namelen = 0;
	    if (!*p)
		break;
	}
    }
}

void DocComment::writeSections(Scope *sc, Dsymbol *s, OutBuffer *buf)
{
    //printf("DocComment::writeSections()\n");
    if (sections.dim)
    {
	buf->writestring("$(DDOC_SECTIONS \n");
	for (int i = 0; i < sections.dim; i++)
	{   Section *sec = (Section *)sections.data[i];

	    if (sec->nooutput)
		continue;
	    //printf("Section: '%.*s' = '%.*s'\n", sec->namelen, sec->name, sec->bodylen, sec->body);
	    if (sec->namelen || i)
		sec->write(this, sc, s, buf);
	    else
	    {
		buf->writestring("$(DDOC_SUMMARY ");
		    unsigned o = buf->offset;
		    buf->write(sec->body, sec->bodylen);
		    highlightText(sc, s, buf, o);
		buf->writestring(")\n");
	    }
	}
	buf->writestring(")\n");
    }
    else
    {
	buf->writestring("$(DDOC_BLANKLINE)\n");
    }
}

/***************************************************
 */

void Section::write(DocComment *dc, Scope *sc, Dsymbol *s, OutBuffer *buf)
{
    if (namelen)
    {
	static char *table[] =
	{	"AUTHORS", "BUGS", "COPYRIGHT", "DATE",
		"DEPRECATED", "EXAMPLES", "HISTORY", "LICENSE",
		"RETURNS", "SEE_ALSO", "STANDARDS", "THROWS",
		"VERSION" };

	for (int i = 0; i < sizeof(table) / sizeof(table[0]); i++)
	{
	    if (icmp(table[i], name, namelen) == 0)
	    {
		buf->printf("$(DDOC_%s ", table[i]);
		goto L1;
	    }
	}

	buf->writestring("$(DDOC_SECTION ");
	    // Replace _ characters with spaces
	    buf->writestring("$(DDOC_SECTION_H ");
	    for (unsigned u = 0; u < namelen; u++)
	    {   unsigned char c = name[u];
		buf->writeByte((c == '_') ? ' ' : c);
	    }
	    buf->writestring(":)\n");
    }
    else
    {
	buf->writestring("$(DDOC_DESCRIPTION ");
    }
  L1:
    unsigned o = buf->offset;
    buf->write(body, bodylen);
    highlightText(sc, s, buf, o);
    buf->writestring(")\n");
}

/***************************************************
 */

void ParamSection::write(DocComment *dc, Scope *sc, Dsymbol *s, OutBuffer *buf)
{
    unsigned char *p = body;
    unsigned len = bodylen;
    unsigned char *pend = p + len;

    unsigned char *tempstart;
    unsigned templen;

    unsigned char *namestart;
    unsigned namelen = 0;	// !=0 if line continuation

    unsigned char *textstart;
    unsigned textlen;

    unsigned o;
    Argument *arg;

    buf->writestring("$(DDOC_PARAMS \n");
    while (p < pend)
    {
	// Skip to start of macro
	for (; 1; p++)
	{
	    switch (*p)
	    {
		case ' ':
		case '\t':
		    continue;

		case '\n':
		    p++;
		    goto Lcont;

		default:
		    if (!(isalpha(*p) || *p == '_'))
		    {
			if (namelen)
			    goto Ltext;		// continuation of prev macro
			goto Lskipline;
		    }
		    break;
	    }
	    break;
	}
	tempstart = p;

	while (isalnum(*p) || *p == '_')
	    p++;
	templen = p - tempstart;

	while (*p == ' ' || *p == '\t')
	    p++;

	if (*p != '=')
	{   if (namelen)
		goto Ltext;		// continuation of prev macro
	    goto Lskipline;
	}
	p++;

	if (namelen)
	{   // Output existing param

	L1:
	    //printf("param '%.*s' = '%.*s'\n", namelen, namestart, textlen, textstart);
	    HdrGenState hgs;
	    buf->writestring("$(DDOC_PARAM_ROW ");
		buf->writestring("$(DDOC_PARAM_ID ");
		    o = buf->offset;
		    arg = isFunctionParameter(s, namestart, namelen);
		    if (arg && arg->type && arg->ident)
			arg->type->toCBuffer(buf, arg->ident, &hgs);
		    else
			buf->write(namestart, namelen);
		    highlightCode(sc, s, buf, o);
		buf->writestring(")\n");

		buf->writestring("$(DDOC_PARAM_DESC ");
		    o = buf->offset;
		    buf->write(textstart, textlen);
		    highlightText(sc, s, buf, o);
		buf->writestring(")");
	    buf->writestring(")\n");
	    namelen = 0;
	    if (p >= pend)
		break;
	}

	namestart = tempstart;
	namelen = templen;

	while (*p == ' ' || *p == '\t')
	    p++;
	textstart = p;

      Ltext:
	while (*p != '\n')
	    p++;
	textlen = p - textstart;
	p++;

     Lcont:
	continue;

     Lskipline:
	// Ignore this line
	while (*p++ != '\n')
	    ;
    }
    if (namelen)
	goto L1;		// write out last one
    buf->writestring(")\n");
}

/***************************************************
 */

void MacroSection::write(DocComment *dc, Scope *sc, Dsymbol *s, OutBuffer *buf)
{
    //printf("MacroSection::write()\n");
    DocComment::parseMacros(dc->pmacrotable, body, bodylen);
}

/************************************************
 * Parse macros out of Macros: section.
 * Macros are of the form:
 *	name1 = value1
 *
 *	name2 = value2
 */

void DocComment::parseMacros(Macro **pmacrotable, unsigned char *m, unsigned mlen)
{
    unsigned char *p = m;
    unsigned len = mlen;
    unsigned char *pend = p + len;

    unsigned char *tempstart;
    unsigned templen;

    unsigned char *namestart;
    unsigned namelen = 0;	// !=0 if line continuation

    unsigned char *textstart;
    unsigned textlen;

    while (p < pend)
    {
	// Skip to start of macro
	for (; 1; p++)
	{
	    if (p >= pend)
		goto Ldone;
	    switch (*p)
	    {
		case ' ':
		case '\t':
		    continue;

		case '\n':
		    p++;
		    goto Lcont;

		default:
		    if (!(isalpha(*p) || *p == '_'))
		    {
			if (namelen)
			    goto Ltext;		// continuation of prev macro
			goto Lskipline;
		    }
		    break;
	    }
	    break;
	}
	tempstart = p;

	while (1)
	{
	    if (p >= pend)
		goto Ldone;
	    if (!(isalnum(*p) || *p == '_'))
		break;
	    p++;
	}
	templen = p - tempstart;

	while (1)
	{
	    if (p >= pend)
		goto Ldone;
	    if (!(*p == ' ' || *p == '\t'))
		break;
	    p++;
	}

	if (*p != '=')
	{   if (namelen)
		goto Ltext;		// continuation of prev macro
	    goto Lskipline;
	}
	p++;
	if (p >= pend)
	    goto Ldone;

	if (namelen)
	{   // Output existing macro
	L1:
	    //printf("macro '%.*s' = '%.*s'\n", namelen, namestart, textlen, textstart);
	    Macro::define(pmacrotable, namestart, namelen, textstart, textlen);
	    namelen = 0;
	    if (p >= pend)
		break;
	}

	namestart = tempstart;
	namelen = templen;

	while (p < pend && (*p == ' ' || *p == '\t'))
	    p++;
	textstart = p;

      Ltext:
	while (p < pend && *p != '\n')
	    p++;
	textlen = p - textstart;

	// Remove trailing \r if there is one
	if (p > m && p[-1] == '\r')
	    textlen--;

	p++;
	//printf("p = %p, pend = %p\n", p, pend);

     Lcont:
	continue;

     Lskipline:
	// Ignore this line
	while (p < pend && *p++ != '\n')
	    ;
    }
Ldone:
    if (namelen)
	goto L1;		// write out last one
}

/******************************************
 * Compare 0-terminated string with length terminated string.
 * Return < 0, ==0, > 0
 */

int cmp(char *stringz, void *s, size_t slen)
{
    size_t len1 = strlen(stringz);

    if (len1 != slen)
	return len1 - slen;
    return memcmp(stringz, s, slen);
}

int icmp(char *stringz, void *s, size_t slen)
{
    size_t len1 = strlen(stringz);

    if (len1 != slen)
	return len1 - slen;
    return memicmp(stringz, (char *)s, slen);
}

/*****************************************
 * Return !=0 if comment consists entirely of "ditto".
 */

int isDitto(unsigned char *comment)
{
    if (comment)
    {
	unsigned char *p = skipwhitespace(comment);

	if (memicmp((char *)p, "ditto", 5) == 0 && *skipwhitespace(p + 5) == 0)
	    return 1;
    }
    return 0;
}

/**********************************************
 * Skip white space.
 */

unsigned char *skipwhitespace(unsigned char *p)
{
    for (; 1; p++)
    {	switch (*p)
	{
	    case ' ':
	    case '\t':
	    case '\n':
		continue;
	}
	break;
    }
    return p;
}


/************************************************
 * Scan forward to one of:
 *	start of identifier
 *	beginning of next line
 *	end of buf
 */

unsigned skiptoident(OutBuffer *buf, unsigned i)
{
    for (; i < buf->offset; i++)
    {
	// BUG: handle unicode alpha's
	unsigned char c = buf->data[i];
	if (isalpha(c) || c == '_')
	    break;
	if (c == '\n')
	    break;
    }
    return i;
}

/************************************************
 * Scan forward past end of identifier.
 */

unsigned skippastident(OutBuffer *buf, unsigned i)
{
    for (; i < buf->offset; i++)
    {
	// BUG: handle unicode alpha's
	unsigned char c = buf->data[i];
	if (!(isalnum(c) || c == '_'))
	    break;
    }
    return i;
}


/****************************************************
 */

int isKeyword(unsigned char *p, unsigned len)
{
    static char *table[] = { "true", "false", "null" };

    for (int i = 0; i < sizeof(table) / sizeof(table[0]); i++)
    {
	if (cmp(table[i], p, len) == 0)
	    return 1;
    }
    return 0;
}

/****************************************************
 */

Argument *isFunctionParameter(Dsymbol *s, unsigned char *p, unsigned len)
{
    FuncDeclaration *f = s->isFuncDeclaration();

    /* f->type may be NULL for template members.
     */
    if (f && f->type)
    {
	TypeFunction *tf = (TypeFunction *)f->type;

	if (tf->arguments)
	{
	    for (int k = 0; k < tf->arguments->dim; k++)
	    {   Argument *arg = (Argument *)tf->arguments->data[k];

		if (arg->ident && cmp(arg->ident->toChars(), p, len) == 0)
		{
		    return arg;
		}
	    }
	}
    }
    return NULL;
}

/**************************************************
 * Highlight text section.
 */

void highlightText(Scope *sc, Dsymbol *s, OutBuffer *buf, unsigned offset)
{
    //printf("highlightText()\n");
    char *sid = s->ident->toChars();
    FuncDeclaration *f = s->isFuncDeclaration();
    unsigned char *p;

    int leadingBlank = 1;
    int inCode = 0;
    int inComment = 0;			// in <!-- ... --> comment
    unsigned iCodeStart;		// start of code section

    unsigned iLineStart = offset;

    for (unsigned i = offset; i < buf->offset; i++)
    {	unsigned char c = buf->data[i];

     Lcont:
	switch (c)
	{
	    case ' ':
	    case '\t':
		break;

	    case '\n':
		if (sc && !inCode && i == iLineStart && i + 1 < buf->offset)	// if "\n\n"
		{
		    static char blankline[] = "$(DDOC_BLANKLINE)\n";

		    i = buf->insert(i, blankline, sizeof(blankline) - 1);
		}
		leadingBlank = 1;
		iLineStart = i + 1;
		break;

	    case '<':
		leadingBlank = 0;
		if (inCode)
		    break;
		p = &buf->data[i];

		// Skip over comments
		if (p[1] == '!' && p[2] == '-' && p[3] == '-')
		{   unsigned j = i + 4;
		    p += 4;
		    while (1)
		    {
			if (j == buf->offset)
			    goto L1;
			if (p[0] == '-' && p[1] == '-' && p[2] == '>')
			{
			    i = j + 2;	// place on closing '>'
			    break;
			}
			j++;
			p++;
		    }
		    break;
		}

		// Skip over HTML tag
		if (isalpha(p[1]) || (p[1] == '/' && isalpha(p[2])))
		{   unsigned j = i + 2;
		    p += 2;
		    while (1)
		    {
			if (j == buf->offset)
			    goto L1;
			if (p[0] == '>')
			{
			    i = j;	// place on closing '>'
			    break;
			}
			j++;
			p++;
		    }
		    break;
		}

	    L1:
		// Replace '<' with &lt; character entity
		buf->remove(i, 1);
		buf->insert(i, "&lt;", 4);
		i += 3;		// point to ';'
		break;

	    case '>':
		leadingBlank = 0;
		if (inCode)
		    break;
		buf->remove(i, 1);
		i = buf->insert(i, "&gt;", 4);
		i--;		// point to ';'
		break;

	    case '&':
		leadingBlank = 0;
		if (inCode)
		    break;
		p = &buf->data[i];
		if (p[1] == '#' || isalpha(p[1]))
		    break;			// already a character entity
		buf->remove(i, 1);
		i = buf->insert(i, "&amp;", 5);
		i--;		// point to ';'
		break;

	    case '-':
		/* A line beginning with --- delimits a code section.
		 * inCode tells us if it is start or end of a code section.
		 */
		if (leadingBlank)
		{   int istart = i;
		    int eollen = 0;

		    leadingBlank = 0;
		    while (1)
		    {
			++i;
			if (i >= buf->offset)
			    break;
			c = buf->data[i];
			if (c == '\n')
			{   eollen = 1;
			    break;
			}
			if (c == '\r')
			{
			    eollen = 1;
			    if (i + 1 >= buf->offset)
				break;
			    if (buf->data[i + 1] == '\n')
			    {	eollen = 2;
				break;
			    }
			}
			// BUG: handle UTF PS and LS too
			if (c != '-')
			    goto Lcont;
		    }
		    if (i - istart < 3)
			goto Lcont;

		    // We have the start/end of a code section

		    // Remove the entire --- line, including blanks and \n
		    buf->remove(iLineStart, i - iLineStart + eollen);
		    i = iLineStart;

		    if (inCode)
		    {
			inCode = 0;
			// The code section is from iCodeStart to i
			OutBuffer codebuf;

			codebuf.write(buf->data + iCodeStart, i - iCodeStart);
			codebuf.writeByte(0);
			highlightCode2(sc, s, &codebuf, 0);
			buf->remove(iCodeStart, i - iCodeStart);
			i = buf->insert(iCodeStart, codebuf.data, codebuf.offset);
			i = buf->insert(i, ")\n", 2);
			i--;
		    }
		    else
		    {	static char pre[] = "$(D_CODE \n";

			inCode = 1;
			i = buf->insert(i, pre, sizeof(pre) - 1);
			iCodeStart = i;
			i--;		// place i on >
		    }
		}
		break;

	    default:
		leadingBlank = 0;
		if (sc && !inCode && (isalpha(c) || c == '_'))
		{   unsigned j;

		    j = skippastident(buf, i);
		    if (j > i)
		    {
			if (buf->data[i] == '_')	// leading '_' means no highlight
			{
			    buf->remove(i, 1);
			    i = j - 1;
			}
			else
			{
			    if (cmp(sid, buf->data + i, j - i) == 0)
			    {
				i = buf->bracket(i, "$(DDOC_PSYMBOL ", j, ")") - 1;
				break;
			    }
			    else if (isKeyword(buf->data + i, j - i))
			    {
				i = buf->bracket(i, "$(DDOC_KEYWORD ", j, ")") - 1;
				break;
			    }
			    else
			    {
				if (f && isFunctionParameter(f, buf->data + i, j - i))
				{
				    //printf("highlighting arg '%s', i = %d, j = %d\n", arg->ident->toChars(), i, j);
				    i = buf->bracket(i, "$(DDOC_PARAM ", j, ")") - 1;
				    break;
				}
			    }
			    i = j - 1;
			}
		    }
		}
		break;
	}
    }
  Ldone:
    ;
}

/**************************************************
 * Highlight code for DDOC section.
 */

void highlightCode(Scope *sc, Dsymbol *s, OutBuffer *buf, unsigned offset)
{
    char *sid = s->ident->toChars();
    FuncDeclaration *f = s->isFuncDeclaration();

    //printf("highlightCode(s = '%s', kind = %s)\n", sid, s->kind());
    for (unsigned i = offset; i < buf->offset; i++)
    {	unsigned char c = buf->data[i];

	switch (c)
	{
	    case '<':			// replace '<' with '&lt;'
		buf->remove(i, 1);
		i = buf->insert(i, "&lt;", 4);
		i--;		// point to ';'
		break;

	    case '>':
		buf->remove(i, 1);
		i = buf->insert(i, "&gt;", 4);
		i--;		// point to ';'
		break;

	    case '&':
		buf->remove(i, 1);
		i = buf->insert(i, "&amp;", 5);
		i--;		// point to ';'
		break;

	    default:
		if (isalpha(c) || c == '_')
		{   unsigned j;

		    j = skippastident(buf, i);
		    if (j > i)
		    {
			if (cmp(sid, buf->data + i, j - i) == 0)
			{
			    i = buf->bracket(i, "$(DDOC_PSYMBOL ", j, ")") - 1;
			    break;
			}
			else if (f)
			{
			    if (isFunctionParameter(f, buf->data + i, j - i))
			    {
				//printf("highlighting arg '%s', i = %d, j = %d\n", arg->ident->toChars(), i, j);
				i = buf->bracket(i, "$(DDOC_PARAM ", j, ")") - 1;
				break;
			    }
			}
			i = j - 1;
		    }
		}
		break;
	}
    }
}

/****************************************
 */

void highlightCode3(OutBuffer *buf, unsigned char *p, unsigned char *pend)
{
    for (; p < pend; p++)
    {
	switch (*p)
	{
	    case '<':
		buf->writestring("&lt;");
		break;
	    case '>':
		buf->writestring("&gt;");
		break;
	    case '&':
		buf->writestring("&amp;");
		break;
	    default:
		buf->writeByte(*p);
		break;
	}
    }
}

/**************************************************
 * Highlight code for CODE section.
 */


void highlightCode2(Scope *sc, Dsymbol *s, OutBuffer *buf, unsigned offset)
{
    char *sid = s->ident->toChars();
    FuncDeclaration *f = s->isFuncDeclaration();
    unsigned errorsave = global.errors;
    Lexer lex(NULL, buf->data, 0, buf->offset - 1, 0, 1);
    Token tok;
    OutBuffer res;
    unsigned char *lastp = buf->data;
    char *highlight;

    //printf("highlightCode2('%.*s')\n", buf->offset - 1, buf->data);
    res.reserve(buf->offset);
    while (1)
    {
	lex.scan(&tok);
	highlightCode3(&res, lastp, tok.ptr);
	highlight = NULL;
	switch (tok.value)
	{
	    case TOKidentifier:
		if (!sc)
		    break;
		if (cmp(sid, tok.ptr, lex.p - tok.ptr) == 0)
		{
		    highlight = "$(D_PSYMBOL ";
		    break;
		}
		else if (f)
		{
		    if (isFunctionParameter(f, tok.ptr, lex.p - tok.ptr))
		    {
			//printf("highlighting arg '%s', i = %d, j = %d\n", arg->ident->toChars(), i, j);
			highlight = "$(D_PARAM ";
			break;
		    }
		}
		break;

	    case TOKcomment:
		highlight = "$(D_COMMENT ";
		break;

	    case TOKstring:
		highlight = "$(D_STRING ";
		break;

	    default:
		if (tok.isKeyword())
		    highlight = "$(D_KEYWORD ";
		break;
	}
	if (highlight)
	    res.writestring(highlight);
	highlightCode3(&res, tok.ptr, lex.p);
	if (highlight)
	    res.writeByte(')');
	if (tok.value == TOKeof)
	    break;
	lastp = lex.p;
    }
    buf->setsize(offset);
    buf->write(&res);
    global.errors = errorsave;
}
