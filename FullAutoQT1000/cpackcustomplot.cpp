﻿#include "cpackcustomplot.h"

CPackCustomPlot::CPackCustomPlot(QCustomPlot* plot)
{
    m_plot = plot;
    m_bOverlay = true;
}

void CPackCustomPlot::InitPlotParam()
{
    m_plot->xAxis->setLabel("Raw Data To X");
    m_plot->yAxis->setLabel("Raw Data To Y");
    m_plot->xAxis->setRange(0,320);
    m_plot->yAxis->setRange(0,5000);
}

void CPackCustomPlot::InitPlotParam(int yAxisHight)
{
    m_plot->xAxis->setLabel("Raw Data To X");
    m_plot->yAxis->setLabel("Raw Data To Y");
    m_plot->xAxis->setRange(0,320);
    m_plot->yAxis->setRange(0,yAxisHight);
}

void CPackCustomPlot::InitPlotParam(int xAxisLow, int xAxisHight, int yAxisLow, int yAxisHight)
{
    m_plot->xAxis->setLabel("Raw Data To X");
    m_plot->yAxis->setLabel("Raw Data To Y");
    m_plot->xAxis->setRange(xAxisLow,xAxisHight);
    m_plot->yAxis->setRange(yAxisLow,yAxisHight);
}

void CPackCustomPlot::DrawGraph(QVector<double> vecData)
{
    if(m_bOverlay == false){
        ClearGraph();
    }
    //qDebug()<<byteData.size();
    SetGraphData(vecData);
    m_PointX.clear();
    m_PointY.clear();
    m_plot->addGraph();
    int nXindex = 1;
    for(int n = 0; n<vecData.size(); n++)
    {
        m_PointX.append(nXindex);
        nXindex++;
    }
    m_PointY = m_vecRawData;

    QColor LineColor = QColor(qrand()%255,qrand()%255,qrand()%255);
    m_plot->graph()->setPen(QPen(LineColor, LineWidth));

    QColor DotColor = QColor(qrand()%255,qrand()%255,qrand()%255);
    m_plot->graph()->setScatterStyle(
                QCPScatterStyle(QCPScatterStyle::ssCircle,
                                QPen(DotColor, LineWidth),
                                QBrush(DotColor), DotWidth));

    m_plot->graph()->setData(m_PointX,m_PointY);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_plot->replot();
}

void CPackCustomPlot::SetGraphData(QVector<double> vecData)
{
    m_vecRawData.clear();
    m_vecRawData = vecData;
}

void CPackCustomPlot::ClearGraph()
{
    m_plot->clearGraphs();
    m_plot->replot();
}

void CPackCustomPlot::SetOverlay(bool bOverlay)
{
    m_bOverlay = bOverlay;
}
