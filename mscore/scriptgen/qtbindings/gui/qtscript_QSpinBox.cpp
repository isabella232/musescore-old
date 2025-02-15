#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <qspinbox.h>
#include <QVariant>
#include <qaction.h>
#include <qbitmap.h>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qcursor.h>
#include <qevent.h>
#include <qfont.h>
#include <qgraphicsproxywidget.h>
#include <qicon.h>
#include <qinputcontext.h>
#include <qkeysequence.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlocale.h>
#include <qobject.h>
#include <qpaintdevice.h>
#include <qpaintengine.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpoint.h>
#include <qrect.h>
#include <qregion.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qspinbox.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qwidget.h>

#include "qtscriptshell_QSpinBox.h"

static const char * const qtscript_QSpinBox_function_names[] = {
    "QSpinBox"
    // static
    // prototype
    , "setRange"
    , "toString"
};

static const char * const qtscript_QSpinBox_function_signatures[] = {
    "QWidget parent"
    // static
    // prototype
    , "int min, int max"
""
};

static QScriptValue qtscript_QSpinBox_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("QSpinBox::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(QSpinBox*)
Q_DECLARE_METATYPE(QtScriptShell_QSpinBox*)
Q_DECLARE_METATYPE(QAbstractSpinBox*)

//
// QSpinBox
//

static QScriptValue qtscript_QSpinBox_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 1;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    QSpinBox* _q_self = qscriptvalue_cast<QSpinBox*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("QSpinBox.%0(): this object is not a QSpinBox")
            .arg(qtscript_QSpinBox_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 2) {
        int _q_arg0 = context->argument(0).toInt32();
        int _q_arg1 = context->argument(1).toInt32();
        _q_self->setRange(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 1: {
    QString result = QString::fromLatin1("QSpinBox");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_QSpinBox_throw_ambiguity_error_helper(context,
        qtscript_QSpinBox_function_names[_id+1],
        qtscript_QSpinBox_function_signatures[_id+1]);
}

static QScriptValue qtscript_QSpinBox_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("QSpinBox(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QtScriptShell_QSpinBox* _q_cpp_result = new QtScriptShell_QSpinBox();
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (QSpinBox*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 1) {
        QWidget* _q_arg0 = qscriptvalue_cast<QWidget*>(context->argument(0));
        QtScriptShell_QSpinBox* _q_cpp_result = new QtScriptShell_QSpinBox(_q_arg0);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (QSpinBox*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_QSpinBox_throw_ambiguity_error_helper(context,
        qtscript_QSpinBox_function_names[_id],
        qtscript_QSpinBox_function_signatures[_id]);
}

static QScriptValue qtscript_QSpinBox_toScriptValue(QScriptEngine *engine, QSpinBox* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_QSpinBox_fromScriptValue(const QScriptValue &value, QSpinBox* &out)
{
    out = qobject_cast<QSpinBox*>(value.toQObject());
}

QScriptValue qtscript_create_QSpinBox_class(QScriptEngine *engine)
{
    static const int function_lengths[] = {
        1
        // static
        // prototype
        , 2
        , 0
    };
    engine->setDefaultPrototype(qMetaTypeId<QSpinBox*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((QSpinBox*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QAbstractSpinBox*>()));
    for (int i = 0; i < 2; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_QSpinBox_prototype_call, function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_QSpinBox_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    qScriptRegisterMetaType<QSpinBox*>(engine, qtscript_QSpinBox_toScriptValue, 
        qtscript_QSpinBox_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_QSpinBox_static_call, proto, function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
