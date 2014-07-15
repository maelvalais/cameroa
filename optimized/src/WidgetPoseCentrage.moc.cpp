/****************************************************************************
** ObjPoseCentrage meta object code from reading C++ file 'WidgetPoseCentrage.h'
**
** Created: Wed Oct 25 10:40:05 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../src/WidgetPoseCentrage.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ObjPoseCentrage::className() const
{
    return "ObjPoseCentrage";
}

QMetaObject *ObjPoseCentrage::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ObjPoseCentrage( "ObjPoseCentrage", &ObjPoseCentrage::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ObjPoseCentrage::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ObjPoseCentrage", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ObjPoseCentrage::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ObjPoseCentrage", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ObjPoseCentrage::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = ObjPoseImage::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"ObjPoseCentrage", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ObjPoseCentrage.setMetaObject( metaObj );
    return metaObj;
}

void* ObjPoseCentrage::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ObjPoseCentrage" ) )
	return this;
    return ObjPoseImage::qt_cast( clname );
}

bool ObjPoseCentrage::qt_invoke( int _id, QUObject* _o )
{
    return ObjPoseImage::qt_invoke(_id,_o);
}

bool ObjPoseCentrage::qt_emit( int _id, QUObject* _o )
{
    return ObjPoseImage::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ObjPoseCentrage::qt_property( int id, int f, QVariant* v)
{
    return ObjPoseImage::qt_property( id, f, v);
}

bool ObjPoseCentrage::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
