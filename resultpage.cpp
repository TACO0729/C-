#include "resultpage.h"
#include "ui_resultpage.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDateTime>
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QPushButton>
#include <QPixmap>
#include <QFontDatabase>
#include <QPalette>
#include <QtMath>

static QString chooseNiceFont()
{
    QFontDatabase db;
    QStringList families = db.families();

    if (families.contains("YouYuan")) {
        return "YouYuan";
    }

    if (families.contains("幼圆")) {
        return "幼圆";
    }

    if (families.contains("Microsoft YaHei UI")) {
        return "Microsoft YaHei UI";
    }

    if (families.contains("Microsoft YaHei")) {
        return "Microsoft YaHei";
    }

    if (families.contains("微软雅黑")) {
        return "微软雅黑";
    }

    if (families.contains("SimHei")) {
        return "SimHei";
    }

    if (families.contains("黑体")) {
        return "黑体";
    }

    return QApplication::font().family();
}

DimensionBar::DimensionBar(char dim, int score, int maxScore, QWidget* parent)
    : QFrame(parent),
    m_dim(dim),
    m_score(score),
    m_maxScore(maxScore)
{
    setMinimumHeight(58);
    setMaximumHeight(58);
    setFrameStyle(QFrame::NoFrame);

    // 避免被父控件 QSS 边框影响
    setStyleSheet("background: transparent; border: none;");
}

QString DimensionBar::preferredFontFamily() const
{
    return chooseNiceFont();
}

void DimensionBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int w = width();
    const int h = height();

    QColor barColor;

    switch (m_dim) {
    case 'E':
        barColor = QColor("#7b67c7");
        break;
    case 'S':
        barColor = QColor("#5ca0c4");
        break;
    case 'T':
        barColor = QColor("#e5b13f");
        break;
    case 'J':
        barColor = QColor("#5fa477");
        break;
    default:
        barColor = QColor("#7b67c7");
        break;
    }

    QString leftText = MBTIEngine::getDimensionLeft(m_dim);
    QString rightText = MBTIEngine::getDimensionRight(m_dim);

    QFont textFont(preferredFontFamily());
    textFont.setPointSize(12);
    textFont.setBold(true);
    p.setFont(textFont);
    p.setPen(QColor("#202020"));

    // 左右文字区域
    int leftTextX = 0;
    int leftTextW = 90;
    int rightTextW = 90;

    int barX = leftTextX + leftTextW + 15;
    int barW = w - leftTextW - rightTextW - 35;
    int barH = 16;
    int barY = (h - barH) / 2;

    if (barW < 150) {
        barW = 150;
    }

    // 左侧文字
    p.drawText(leftTextX, 0, leftTextW, h, Qt::AlignVCenter | Qt::AlignRight, leftText);

    // 右侧文字
    p.drawText(barX + barW + 15, 0, rightTextW, h, Qt::AlignVCenter | Qt::AlignLeft, rightText);

    // 灰色背景条
    QRectF bgRect(barX, barY, barW, barH);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#e9e9e9"));
    p.drawRoundedRect(bgRect, barH / 2, barH / 2);

    int absScore = qAbs(m_score);

    if (m_maxScore <= 0) {
        m_maxScore = 8;
    }

    double ratio = static_cast<double>(absScore) / static_cast<double>(m_maxScore);
    ratio = qBound(0.0, ratio, 1.0);

    // 保证分数不为 0 时，彩色条至少有一点长度
    int fillW = static_cast<int>(barW * ratio);

    if (absScore > 0 && fillW < 35) {
        fillW = 35;
    }

    if (fillW > barW) {
        fillW = barW;
    }

    QRectF fillRect;

    // m_score >= 0 表示偏向左侧维度，彩色条从左向右
    // m_score < 0 表示偏向右侧维度，彩色条从右向左
    if (m_score >= 0) {
        fillRect = QRectF(barX, barY, fillW, barH);
    }
    else {
        fillRect = QRectF(barX + barW - fillW, barY, fillW, barH);
    }

    if (absScore > 0) {
        p.setBrush(barColor);
        p.drawRoundedRect(fillRect, barH / 2, barH / 2);
    }

    // 分数气泡
    QString scoreText = QString::number(absScore);

    QFont scoreFont(preferredFontFamily());
    scoreFont.setPointSize(10);
    scoreFont.setBold(true);
    p.setFont(scoreFont);

    int bubbleW = qMax(38, p.fontMetrics().horizontalAdvance(scoreText) + 18);
    int bubbleH = 24;

    int bubbleCenterX;

    if (m_score >= 0) {
        bubbleCenterX = barX + fillW;
    }
    else {
        bubbleCenterX = barX + barW - fillW;
    }

    // 防止气泡跑出进度条范围
    bubbleCenterX = qBound(barX + bubbleW / 2, bubbleCenterX, barX + barW - bubbleW / 2);

    QRectF bubbleRect(
        bubbleCenterX - bubbleW / 2,
        barY + barH / 2 - bubbleH / 2,
        bubbleW,
        bubbleH
    );

    // 分数为 0 时也显示在中间
    if (absScore == 0) {
        bubbleRect.moveCenter(QPointF(barX + barW / 2, barY + barH / 2));
    }

    p.setBrush(barColor);
    p.setPen(QPen(QColor("#ffffff"), 2));
    p.drawRoundedRect(bubbleRect, bubbleH / 2, bubbleH / 2);

    p.setPen(QColor("#ffffff"));
    p.drawText(bubbleRect, Qt::AlignCenter, scoreText);
}

ResultPage::ResultPage(QWidget* parent)
    : QWidget(parent),
    ui(new Ui::ResultPage)
{
    ui->setupUi(this);

    // 让整个页面都是白色，防止底部按钮两边出现黑色
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);

    // 设置整体字体：优先幼圆，没有就自动用类似字体
    QString fontFamily = preferredFontFamily();
    QFont font(fontFamily);
    setFont(font);

    QString globalStyle = QString(
        "QWidget#ResultPage {"
        "background-color: white;"
        "font-family: \"%1\", \"YouYuan\", \"幼圆\", \"Microsoft YaHei\", \"微软雅黑\", \"SimHei\", \"黑体\";"
        "}"
        "QLabel {"
        "font-family: \"%1\", \"YouYuan\", \"幼圆\", \"Microsoft YaHei\", \"微软雅黑\", \"SimHei\", \"黑体\";"
        "}"
    ).arg(fontFamily);

    this->setStyleSheet(this->styleSheet() + globalStyle);

    connect(ui->btnRestart, &QPushButton::clicked,
        this, &ResultPage::on_btnRestart_clicked);
}

ResultPage::~ResultPage()
{
    delete ui;
}

QString ResultPage::preferredFontFamily() const
{
    return chooseNiceFont();
}

QString ResultPage::colorForType(const QString& typeCode) const
{
    QString t = typeCode.toUpper();

    // NF 组：绿色
    if (t == "ENFJ" || t == "ENFP" ||
        t == "INFJ" || t == "INFP") {
        return "#2e9d57";
    }

    // NT 组：紫色
    if (t == "ENTJ" || t == "ENTP" ||
        t == "INTJ" || t == "INTP") {
        return "#6b5b95";
    }

    // SP 组：黄色
    if (t == "ESFP" || t == "ESTP" ||
        t == "ISFP" || t == "ISTP") {
        return "#e5a927";
    }

    // SJ 组：蓝色
    if (t == "ESFJ" || t == "ESTJ" ||
        t == "ISFJ" || t == "ISTJ") {
        return "#3f7fc4";
    }

    return "#6b5b95";
}

void ResultPage::setResult(const QuizResult& result)
{
    QString typeCode = result.getTypeCode().toUpper();
    QString fontFamily = preferredFontFamily();

    // 时间
   // ui->labelTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    // 报告编号
    QString reportId =
        QDateTime::currentDateTime().toString("yyMMddhhmmss") +
        QString::number(QDateTime::currentMSecsSinceEpoch() % 1000000);

    ui->labelReportId->setText("报告编号：" + reportId);

    // 类型颜色
    QString typeColor = colorForType(typeCode);

    // 最终结果
    // 如果你不想显示 -A，可以把下面这一行改成：ui->labelTypeCode->setText(typeCode);
    ui->labelTypeCode->setText(typeCode + "-A");

    ui->labelTypeCode->setStyleSheet(QString(
        "QLabel {"
        "color: %1;"
        "font-family: \"%2\", \"YouYuan\", \"幼圆\", \"Microsoft YaHei\", \"微软雅黑\", \"SimHei\", \"黑体\";"
        "font-weight: bold;"
        "}"
    ).arg(typeColor, fontFamily));

    // 昵称和描述
    ui->labelNickname->setText(MBTIEngine::getTypeNickname(typeCode));
    ui->labelDescription->setText(MBTIEngine::getTypeDescription(typeCode));

    ui->labelNickname->setStyleSheet(QString(
        "QLabel {"
        "color: %1;"
        "font-family: \"%2\", \"YouYuan\", \"幼圆\", \"Microsoft YaHei\", \"微软雅黑\", \"SimHei\", \"黑体\";"
        "font-weight: bold;"
        "}"
    ).arg(typeColor, fontFamily));

    ui->labelDescription->setStyleSheet(QString(
        "QLabel {"
        "color: %1;"
        "font-family: \"%2\", \"YouYuan\", \"幼圆\", \"Microsoft YaHei\", \"微软雅黑\", \"SimHei\", \"黑体\";"
        "line-height: 150%;"
        "}"
    ).arg(typeColor, fontFamily));

    // 加载人物图片
    QString imagePath = ":/" + typeCode.toLower() + ".png";
    QPixmap pixmap(imagePath);

    if (!pixmap.isNull()) {
        ui->labelImage->setPixmap(
            pixmap.scaled(180, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    }
    else {
        ui->labelImage->setText("图片缺失");
        ui->labelImage->setStyleSheet(QString(
            "QLabel {"
            "color: #999999;"
            "font-family: \"%1\", \"YouYuan\", \"幼圆\", \"Microsoft YaHei\", \"微软雅黑\";"
            "}"
        ).arg(fontFamily));
    }

    // 清空旧的维度条
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->frameDimensions->layout());

    if (!layout) {
        layout = new QVBoxLayout(ui->frameDimensions);
        ui->frameDimensions->setLayout(layout);
    }

    QLayoutItem* child = nullptr;

    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    layout->setContentsMargins(5, 15, 5, 15);
    layout->setSpacing(14);

    // 这里 maxScore 用 8。
    // 如果你的每个维度最大分不是 8，比如是 10 或 12，就改这里。
    int maxScore = 8;

    layout->addWidget(new DimensionBar('E', result.scoreE_I, maxScore, ui->frameDimensions));
    layout->addWidget(new DimensionBar('S', result.scoreS_N, maxScore, ui->frameDimensions));
    layout->addWidget(new DimensionBar('T', result.scoreT_F, maxScore, ui->frameDimensions));
    layout->addWidget(new DimensionBar('J', result.scoreJ_P, maxScore, ui->frameDimensions));
    layout->addStretch();
}

void ResultPage::on_btnRestart_clicked()
{
    emit restartQuiz();
}