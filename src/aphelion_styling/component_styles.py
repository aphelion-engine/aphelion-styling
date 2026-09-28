"""Reusable styles for individual editor-independent UI components."""

SELECTION_BAR_STYLE = """
    QFrame#SelectionActionBar {
        background-color: rgba(26, 26, 32, 235);
        border: 1px solid #383842;
        border-radius: 8px;
    }
    QLabel#SelectionActionCount {
        color: #eaeaf0;
        font-size: 11px;
        font-weight: 600;
        padding: 0 4px 0 2px;
    }
    QLabel#SelectionActionDivider {
        background-color: #3a3a44;
        max-width: 1px;
        min-width: 1px;
    }
    QToolButton#SelectionActionButton {
        background-color: transparent;
        border: 1px solid transparent;
        border-radius: 5px;
        padding: 3px;
    }
    QToolButton#SelectionActionButton:hover:enabled {
        background-color: #2b6ea8;
        border-color: #3d84c0;
    }
    QToolButton#SelectionActionButton:checked {
        background-color: #2b6ea8;
        border-color: #4a95d6;
    }
    QToolButton#SelectionActionButton:disabled {
        background-color: transparent;
    }
"""

CUSTOM_NODE_DIALOG_STYLE = """
    QDialog#CustomNodeDialog {
        background-color: #1a1a1e;
        color: #e6e6e6;
    }
    QDialog#CustomNodeDialog QLabel {
        color: #c8c8d0;
        font-size: 12px;
    }
    QDialog#CustomNodeDialog QLabel#CustomNodeTitle {
        color: #f0f0f4;
        font-size: 16px;
        font-weight: 600;
    }
    QDialog#CustomNodeDialog QLabel#CustomNodeSubtitle {
        color: #9a9aa4;
        font-size: 12px;
    }
    QDialog#CustomNodeDialog QGroupBox {
        color: #c8c8d0;
        border: 1px solid #34343c;
        border-radius: 6px;
        margin-top: 8px;
        padding-top: 12px;
        font-size: 11px;
        font-weight: 600;
    }
    QDialog#CustomNodeDialog QGroupBox::title {
        subcontrol-origin: margin;
        left: 10px;
        padding: 0 4px;
    }
    QDialog#CustomNodeDialog QLineEdit,
    QDialog#CustomNodeDialog QComboBox {
        background-color: #222228;
        color: #e6e6e6;
        border: 1px solid #3a3a44;
        border-radius: 4px;
        padding: 4px 8px;
        min-height: 22px;
    }
    QDialog#CustomNodeDialog QToolButton {
        background-color: #2a2a32;
        color: #e6e6e6;
        border: 1px solid #3a3a44;
        border-radius: 4px;
        padding: 2px 8px;
    }
    QDialog#CustomNodeDialog QToolButton:hover {
        background-color: #3a3a44;
    }
    QDialog#CustomNodeDialog QPushButton {
        background-color: #2a2a32;
        color: #e6e6e6;
        border: 1px solid #3a3a44;
        border-radius: 4px;
        padding: 5px 12px;
        min-height: 24px;
    }
    QDialog#CustomNodeDialog QPushButton:hover {
        background-color: #3a3a44;
    }
    QDialog#CustomNodeDialog QScrollArea {
        background: transparent;
        border: none;
    }
    QDialog#CustomNodeDialog QTabWidget::pane {
        border: 1px solid #34343c;
        border-radius: 6px;
        top: -1px;
    }
    QDialog#CustomNodeDialog QTabBar::tab {
        background-color: #222228;
        color: #b8b8c0;
        border: 1px solid #34343c;
        border-bottom: none;
        border-top-left-radius: 5px;
        border-top-right-radius: 5px;
        padding: 5px 12px;
        margin-right: 2px;
    }
    QDialog#CustomNodeDialog QTabBar::tab:selected {
        background-color: #2e2e36;
        color: #f0f0f4;
    }
    QDialog#CustomNodeDialog QSlider::groove:horizontal {
        height: 4px;
        background: #2e2e36;
        border-radius: 2px;
    }
    QDialog#CustomNodeDialog QSlider::handle:horizontal {
        background: #8ab4d8;
        width: 10px;
        margin: -5px 0;
        border-radius: 5px;
    }
"""

__all__ = ["CUSTOM_NODE_DIALOG_STYLE", "SELECTION_BAR_STYLE"]
