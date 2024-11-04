#ifndef PRICURVEINPUTHBOXLAYOUT_H
#define PRICURVEINPUTHBOXLAYOUT_H

#include <QHBoxLayout>
#include <qtextedit.h>

class QLabel;
class QCheckBox;
class QPushButton;

namespace pm
{
class ParamMgr;
}


namespace pr
{
class PriShapeDialog;
class PriCurveTextEdit final : public QTextEdit
{
    Q_OBJECT
public:
    PriCurveTextEdit(PriShapeDialog *parent);
    ~PriCurveTextEdit() override;

    bool is_valid() const;
protected:
    void focusOutEvent(QFocusEvent *event) override;
private:
    void check_text();

private:
    PriShapeDialog *mp_dialog;
    bool m_is_valid = true;
};

class PriCurveInputHBoxLayout final : public QHBoxLayout
{
    Q_OBJECT
public:
    PriCurveInputHBoxLayout(PriShapeDialog *parent, const QString& add_object_type, int count);
    ~PriCurveInputHBoxLayout() override;

public:
    const QString& add_object_type() const;

    QString param_name() const;
    QString input_content() const;

    QString reverse_name() const;
    QString reverse_value() const;

    void set_param_str(const QString& str);
    void set_reverse(bool reverse);

    bool is_valid() const;

private:
    void init();
    void deinit();

signals:
    void remove_current_layout(PriCurveInputHBoxLayout* layout);

private slots:
    void remove_layout();


private:
    PriShapeDialog *mp_dialog;

    QString m_add_object_type; // HINT@leixunyong。要么是equation, 要么是fixed point set
    int m_count;

    QLabel* mp_label = nullptr;
    PriCurveTextEdit* mp_input_widget = nullptr;
    QCheckBox* mp_input_check_box = nullptr;
    QPushButton* mp_remove_input_button = nullptr;
};
}



#endif // PRICURVEINPUTHBOXLAYOUT_H
