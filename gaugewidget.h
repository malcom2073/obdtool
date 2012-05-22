/**************************************************************************
*   Copyright (C) 2010 by Michael Carpenter (malcom2073)                  *
*   malcom2073@gmail.com                                                  *
*                                                                         *
*   This file is a part of OBDToolbox                                     *
*                                                                         *
*   OBDToolbox is free software: you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation, either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   OBDToolbox is distributed in the hope that it will be useful,         *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with OBDToolbox.  If not, see <http://www.gnu.org/licenses/>.   *
***************************************************************************/

#ifndef GAUGEWIDGET_H
#define GAUGEWIDGET_H
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QTimer>
class GaugeWidget : public QWidget
{
	Q_OBJECT
public:
	GaugeWidget(QWidget *parent=0);
	void setValue(double value);
	void passEvent(QStringList evt);
	void setMinimum(double min) { _minValue = min; }
	double minimum() { return _minValue;}
	double maximum() { return _maxValue;}
	double startAngle() { return _scaleStartAngle;}
	double endAngle() { return _scaleEndAngle;}
	double smoothing() { return _smoothValue;}
	double numLabels() { return _numLabels;}
	int fade() { return fadeAmount;}
	void setFadingOn(bool fade);
	void setMaximum(double max) { _maxValue = max; }
	void setStartAngle(double ang) { _scaleStartAngle = ang; }
	void setEndAngle(double ang) { _scaleEndAngle = ang; }
	void setSmoothing(double smooth) { _smoothValue = smooth; }
	void setNumLabels(int num) { _numLabels = num; }
	void setColor(QColor outlinecolor, QColor centercolor, QColor topreflectioncolor, QColor bottomreflectioncolor);
	void setColor1(QColor needlecolor, QColor needleoutlinecolor, QColor normaltickcolor, QColor bigtickcolor, QColor warningtickcolor, QColor dangertickcolor);
	void setColor2(QColor fontcolor, QColor warningfontcolor, QColor dangerfontcolor,QFont font);
	void startDemo(int max);
	void setFade(int fade);
	void show();
	void setFake(bool fake) { m_fake = fake;repaint(); }
	void setGaugeStyle(int style);
private:
	int m_style;
	bool m_fake;
	bool m_fadeEnabled;
	int demoMax;
	int fadeAmount;
	void paintEvent(QPaintEvent *evt);
	void resizeEvent (QResizeEvent *evt);
	bool resizeDraw;
	void drawBackground();
	QImage *_bgImage;
	QImage *_fadedBgImage;
	QTimer *demoTimer;
	QTimer *gaugeTimer;
	QTimer *fadeTimer;
	double _scaleEndAngle;
	double _scaleStartAngle;
	double _maxValue;
	double _minValue;
	double _value;
	double _targetValue;
	double _danger;
	double _warning;
	double _smoothValue;
	bool _reverseOrder;
	int _numLabels;
	QPen outlinePen;
	QPen needleCenterPen;
	QPen needleCenterOutlinePen;
	QPen needlePen;
	QPen needleOutlinePen;
	QPen normalTickPen;
	QPen normalBigTickPen;
	QPen normalFontPen;
	QPen dangerFontPen;
	QPen warningFontPen;
	QPen dangerTickPen;
	QPen dangerBigTickPen;
	QPen warningTickPen;
	QPen warningBigTickPen;
	QFont labelFont;
	int fadeCount;
private slots:
	void timerTick();
	void gaugeTimerTick();
	void fadeTimerTick();
};
#endif //GAUGEWIDGET_H
