#include "drawingcanvas.h"

DrawingCanvas::DrawingCanvas(QWidget *parent)  {
    // Set a minimum size for the canvas
    setMinimumSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    // Set a solid background color
    setStyleSheet("background-color: white; border: 1px solid gray;");
}

void DrawingCanvas::clearPoints(){
    m_points.clear();
    // Trigger a repaint to clear the canvas
    update();
}

void DrawingCanvas::paintLines(){
    /* Todo
     * Implement lines drawing per even pair
    */

    isPaintLinesClicked = true;
    update();
}

void DrawingCanvas::segmentDetection() {
    QPixmap pixmap = this->grab();
    QImage image = pixmap.toImage();

    int width = image.width();
    int height = image.height();
    if (width == 0 || height == 0) return;

    QVector<QVector<int>> pixelMap(height, QVector<int>(width, 0));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb rgb = image.pixel(x, y);
            if (rgb != 0xffffffff)
                pixelMap[y][x] = 1;
        }
    }

    m_windows.clear();
    m_nonEmptyCenters.clear();

    const int windowSize = 3;
    const int halfWin = windowSize / 2;

    for (int y = halfWin; y < height - halfWin; ++y) {
        for (int x = halfWin; x < width - halfWin; ++x) {
            QVector<int> window;
            bool hasPoint = false;

            for (int dy = -halfWin; dy <= halfWin; ++dy) {
                for (int dx = -halfWin; dx <= halfWin; ++dx) {
                    int val = pixelMap[y + dy][x + dx];
                    window.append(val);
                    if (val == 1)
                        hasPoint = true;
                }
            }

            if (hasPoint) {
                m_windows.append(window);
                m_nonEmptyCenters.append(QPoint(x, y));
            }
        }
    }

    qDebug() << "Total non-empty windows:" << m_nonEmptyCenters.size();

    for (int i = 0; i < m_windows.size(); ++i) {
        const QVector<int> &win = m_windows[i];
        int count = 0;
        for (int v : win) count += v;

        QPoint center = m_nonEmptyCenters[i];

        if (count == 1) {
            qDebug() << "Endpoint at" << center;
        } else if (count == 3 || count == 5) {
            qDebug() << "Possible corner at" << center;
        }
    }
}


void DrawingCanvas::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set up the pen and brush for drawing the points
    QPen pen(Qt::blue, 5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::blue));

    // Draw a small circle at each stored point
    for (const QPoint& point : std::as_const(m_points)) {
        painter.drawEllipse(point, 3, 3);
    }

    if(isPaintLinesClicked){
        cout << "paint lines block is called" << endl;
        pen.setColor(Qt::red);
        pen.setWidth(4); // 4-pixel wide line
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);

        // Set the painter's pen to our custom pen.
        painter.setPen(pen);

        for(int i=0;i<m_points.size()-1;i+=2){
            //cout << m_points[i].x() << endl;
            painter.drawLine(m_points[i], m_points[i+1]);
        }
        isPaintLinesClicked = false;

        //return painter pen to blue
        pen.setColor(Qt::blue);
        painter.setPen(pen);
    }
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    // Add the mouse click position to our vector of points
    m_points.append(event->pos());
    // Trigger a repaint
    update();
}


