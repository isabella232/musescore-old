#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <qtextcodecplugin.h>
#include <QVariant>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qlist.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qtextcodec.h>

#include "qtscriptshell_QTextCodecPlugin.h"

static const char * const qtscript_QTextCodecPlugin_function_names[] = {
    "QTextCodecPlugin"
    // static
    // prototype
    , "aliases"
    , "createForMib"
    , "createForName"
    , "mibEnums"
    , "names"
    , "toString"
};

static const char * const qtscript_QTextCodecPlugin_function_signatures[] = {
    "QObject parent"
    // static
    // prototype
    , ""
    , "int mib"
    , "QByteArray name"
    , ""
    , ""
""
};

static QScriptValue qtscript_QTextCodecPlugin_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("QTextCodecPlugin::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(QTextCodecPlugin*)
Q_DECLARE_METATYPE(QtScriptShell_QTextCodecPlugin*)
Q_DECLARE_METATYPE(QList<QByteArray>)
Q_DECLARE_METATYPE(QTextCodec*)
Q_DECLARE_METATYPE(QList<int>)

//
// QTextCodecPlugin
//

static QScriptValue qtscript_QTextCodecPlugin_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 5;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    QTextCodecPlugin* _q_self = qscriptvalue_cast<QTextCodecPlugin*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("QTextCodecPlugin.%0(): this object is not a QTextCodecPlugin")
            .arg(qtscript_QTextCodecPlugin_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        QList<QByteArray> _q_result = _q_self->aliases();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        int _q_arg0 = context->argument(0).toInt32();
        QTextCodec* _q_result = _q_self->createForMib(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        QByteArray _q_arg0 = qscriptvalue_cast<QByteArray>(context->argument(0));
        QTextCodec* _q_result = _q_self->createForName(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QList<int> _q_result = _q_self->mibEnums();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        QList<QByteArray> _q_result = _q_self->names();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 5: {
    QString result = QString::fromLatin1("QTextCodecPlugin");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_QTextCodecPlugin_throw_ambiguity_error_helper(context,
        qtscript_QTextCodecPlugin_function_names[_id+1],
        qtscript_QTextCodecPlugin_function_signatures[_id+1]);
}

static QScriptValue qtscript_QTextCodecPlugin_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("QTextCodecPlugin(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QtScriptShell_QTextCodecPlugin* _q_cpp_result = new QtScriptShell_QTextCodecPlugin();
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (QTextCodecPlugin*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 1) {
        QObject* _q_arg0 = context->argument(0).toQObject();
        QtScriptShell_QTextCodecPlugin* _q_cpp_result = new QtScriptShell_QTextCodecPlugin(_q_arg0);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (QTextCodecPlugin*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_QTextCodecPlugin_throw_ambiguity_error_helper(context,
        qtscript_QTextCodecPlugin_function_names[_id],
        qtscript_QTextCodecPlugin_function_signatures[_id]);
}

static QScriptValue qtscript_QTextCodecPlugin_toScriptValue(QScriptEngine *engine, QTextCodecPlugin* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_QTextCodecPlugin_fromScriptValue(const QScriptValue &value, QTextCodecPlugin* &out)
{
    out = qobject_cast<QTextCodecPlugin*>(value.toQObject());
}

QScriptValue qtscript_create_QTextCodecPlugin_class(QScriptEngine *engine)
{
    static const int function_lengths[] = {
        1
        // static
        // prototype
        , 0
        , 1
        , 1
        , 0
        , 0
        , 0
    };
    engine->setDefaultPrototype(qMetaTypeId<QTextCodecPlugin*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((QTextCodecPlugin*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QObject*>()));
    for (int i = 0; i < 6; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_QTextCodecPlugin_prototype_call, function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_QTextCodecPlugin_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    qScriptRegisterMetaType<QTextCodecPlugin*>(engine, qtscript_QTextCodecPlugin_toScriptValue, 
        qtscript_QTextCodecPlugin_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_QTextCodecPlugin_static_call, proto, function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
