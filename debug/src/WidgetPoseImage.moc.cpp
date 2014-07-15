/****************************************************************************
** ObjPoseImage meta object code from reading C++ file 'WidgetPoseImage.h'
**
** Created: Tue Oct 24 11:04:13 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../src/WidgetPoseImage.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ObjPoseImage::className() const
{
    return "ObjPoseImage";
}

QMetaObject *ObjPoseImage::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ObjPoseImage( "ObjPoseImage", &ObjPoseImage::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ObjPoseImage::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ObjPoseImage", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ObjPoseImage::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ObjPoseImage", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ObjPoseImage::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"ObjPoseImage", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ObjPoseImage.setMetaObject( metaObj );
    return metaObj;
}

void* ObjPoseImage::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ObjPoseImage" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool ObjPoseImage::qt_invoke( int _id, QUObject* _o )
{
    return QWidget::qt_invoke(_id,_o);
}

bool ObjPoseImage::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ObjPoseImage::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool ObjPoseImage::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
