#include <QtGui>
#include "CodeEditor.h"
#include <iostream>
#include <QAbstractItemView>
#include <QCompleter>
#include <QScrollBar>
CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent), compl(0)
{
	this->setFont(QFont("Consolas", 9));
	this->setTabStopWidth(20);

	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());

	while (max >= 10)
	{
		max /= 10;
		++digits;
	}

	int width = fontMetrics().width(QLatin1Char('9')) * digits;

	return width + 10;
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
	{
		lineNumberArea->scroll(0, dy);
	}
	else
	{
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
	}

	if (rect.contains(viewport()->rect()))
	{
		updateLineNumberAreaWidth(0);
	}
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();

	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();

	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);

			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();

		++blockNumber;
	}
}

#pragma region codeCompletion

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
	std::cout << "pressed in editor " << e->key() << std::endl;
	if (compl->popup()->isVisible()) {
		switch (e->key()) {
		case 16777220: // enter key
		case 16777219: // backspace key
		case 32: // space key
			compl->popup()->hide();
			break;
		}
	}
		
		bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space); // CTRL+space
		if (!compl || !isShortcut) // do not process the shortcut when we have a completer
			QPlainTextEdit::keyPressEvent(e);

		const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
		if (!compl || (ctrlOrShift && e->text().isEmpty()))
			return;

		static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
		bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
		QString completionPrefix = textUnderCursor();

		if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3
			|| eow.contains(e->text().right(1)))) {
			compl->popup()->hide();
			return;
		}

		if (completionPrefix != compl->completionPrefix()) {
			compl->setCompletionPrefix(completionPrefix);
			compl->popup()->setCurrentIndex(compl->completionModel()->index(0, 0));
		}
		
		QRect cr = cursorRect();
		cr.setX(20);
		cr.setWidth(compl->popup()->sizeHintForColumn(0) + compl->popup()->verticalScrollBar()->width());
		compl->popup();
		if((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space)
			compl->complete(cr); // popup it up!
}

void CodeEditor::setCompleter(QCompleter *completer)
{
	if (compl)
		QObject::disconnect(completer, 0, this, 0);

	compl = completer;

	if (!compl)
		return;

	compl->setWidget(this);
	compl->setCompletionMode(QCompleter::PopupCompletion);
	compl->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(compl, SIGNAL(activated(QString)),
		this, SLOT(insertCompletion(QString)));
}

void CodeEditor::insertCompletion(const QString &text)
{
	if (compl->widget() != this)
		return;

	QTextCursor tc = textCursor();
	int extra = text.length() - compl->completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(text.right(extra));
	setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const
{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
	if (compl)
		compl->setWidget(this);

	QPlainTextEdit::focusInEvent(e);
}

QCompleter *CodeEditor::getCompleter() const
{
	return compl;
}



#pragma endregion codeCompletion