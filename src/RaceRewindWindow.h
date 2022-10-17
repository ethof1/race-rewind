#include <QMainWindow>

namespace rrewind
{
	/**
	 * The main window class for the Race Rewind application.
	 */
	class RaceRewindWindow : public QMainWindow
	{
	public:
		/**
		 * Constructor
		 * 
		 * @param parent The parent of this window
		 */
		RaceRewindWindow(QWidget* parent = nullptr);

	private:
		/**
		 * Create all UI components for this widget.
		 */
		void setupUi();
	};
}