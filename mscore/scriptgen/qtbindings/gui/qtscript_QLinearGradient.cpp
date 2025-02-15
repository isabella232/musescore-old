#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <qbrush.h>
#include <QVariant>
#include <qbrush.h>
#include <qcolor.h>
#include <qpair.h>
#include <qpoint.h>
#include <qvector.h>

static const char * const qtscript_QLinearGradient_function_names[] = {
    "QLinearGradient"
    // static
    // prototype
    , "finalStop"
    , "setFinalStop"
    , "setStart"
    , "start"
    , "toString"
};

static const char * const qtscript_QLinearGradient_function_signatures[] = {
    "\nQPointF start, QPointF finalStop\nqreal xStart, qreal yStart, qreal xFinalStop, qreal yFinalStop"
    // static
    // prototype
    , ""
    , "QPointF stop\nqreal x, qreal y"
    , "QPointF start\nqreal x, qreal y"
    , ""
""
};

static QScriptValue qtscript_QLinearGradient_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("QLinearGradient::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(QLinearGradient)
Q_DECLARE_METATYPE(QLinearGradient*)
Q_DECLARE_METATYPE(QGradient*)
Q_DECLARE_METATYPE(QGradient)

//
// QLinearGradient
//

static QScriptValue qtscript_QLinearGradient_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 4;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    QLinearGradient* _q_self = qscriptvalue_cast<QLinearGradient*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("QLinearGradient.%0(): this object is not a QLinearGradient")
            .arg(qtscript_QLinearGradient_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        QPointF _q_result = _q_self->finalStop();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QPointF _q_arg0 = qscriptvalue_cast<QPointF>(context->argument(0));
        _q_self->setFinalStop(_q_arg0);
        return context->engine()->undefinedValue();
    }
    if (context->argumentCount() == 2) {
        qreal _q_arg0 = qscriptvalue_cast<qreal>(context->argument(0));
        qreal _q_arg1 = qscriptvalue_cast<qreal>(context->argument(1));
        _q_self->setFinalStop(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        QPointF _q_arg0 = qscriptvalue_cast<QPointF>(context->argument(0));
        _q_self->setStart(_q_arg0);
        return context->engine()->undefinedValue();
    }
    if (context->argumentCount() == 2) {
        qreal _q_arg0 = qscriptvalue_cast<qreal>(context->argument(0));
        qreal _q_arg1 = qscriptvalue_cast<qreal>(context->argument(1));
        _q_self->setStart(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QPointF _q_result = _q_self->start();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 4: {
    QString result = QString::fromLatin1("QLinearGradient");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_QLinearGradient_throw_ambiguity_error_helper(context,
        qtscript_QLinearGradient_function_names[_id+1],
        qtscript_QLinearGradient_function_signatures[_id+1]);
}

static QScriptValue qtscript_QLinearGradient_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("QLinearGradient(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QLinearGradient _q_cpp_result;
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 2) {
        QPointF _q_arg0 = qscriptvalue_cast<QPointF>(context->argument(0));
        QPointF _q_arg1 = qscriptvalue_cast<QPointF>(context->argument(1));
        QLinearGradient _q_cpp_result(_q_arg0, _q_arg1);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 4) {
        qreal _q_arg0 = qscriptvalue_cast<qreal>(context->argument(0));
        qreal _q_arg1 = qscriptvalue_cast<qreal>(context->argument(1));
        qreal _q_arg2 = qscriptvalue_cast<qreal>(context->argument(2));
        qreal _q_arg3 = qscriptvalue_cast<qreal>(context->argument(3));
        QLinearGradient _q_cpp_result(_q_arg0, _q_arg1, _q_arg2, _q_arg3);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_QLinearGradient_throw_ambiguity_error_helper(context,
        qtscript_QLinearGradient_function_names[_id],
        qtscript_QLinearGradient_function_signatures[_id]);
}

QScriptValue qtscript_create_QLinearGradient_class(QScriptEngine *engine)
{
    static const int function_lengths[] = {
        4
        // static
        // prototype
        , 0
        , 2
        , 2
        , 0
        , 0
    };
    engine->setDefaultPrototype(qMetaTypeId<QLinearGradient*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((QLinearGradient*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QGradient*>()));
    for (int i = 0; i < 5; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_QLinearGradient_prototype_call, function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_QLinearGradient_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<QLinearGradient>(), proto);
    engine->setDefaultPrototype(qMetaTypeId<QLinearGradient*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_QLinearGradient_static_call, proto, function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
