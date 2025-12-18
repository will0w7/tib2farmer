# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.3.0] - 2025-12-18

This version fixes a bug, adds some new features, and adds support for the latest version of the game (v.1.8.1). It's possible and expected that the app will require some adjustments if the capture of any values ​​fails; I haven't been able to completely stabilize the pointers (I think).

### Added

*   **Latest game version support:** The app was updated to support the latest game version (v1.8.1).
*   **Star Shard currency support:** The app now supports this new currency (the value is 500 credits per).
*   **Total account value:** Now a total account value is displayed, which adds up the price of everything the app tracks (credits, resources, relics...).

### Fixed

*   **Total value calculation:** There was a bug that caused the value of the relics not to be included in the total value.

### Changed

*   **Prices for skulls and relics:** Skulls 700 cr, human relics 2k, wyrd relics 1.5k, het relics 1.5k and prec relics 2k.

## [0.2.2] - 2025-02-10

### Changed

*   **Code Refactoring:** Splitting code into different files for easy maintenance.

## [0.2.1] - 2025-01-31

### Fixed

*   Fixed a display issue where negative numbers less than 1000 were not displayed correctly.

## [0.2.0] - 2025-01-30

### Added

*   **Pause Functionality:**  You can now pause the timer by pressing the 'P' key.
*   **Experience Per Hour:** Added a display showing the experience gained per hour.

### Changed

*   **Increased Data Update Rate:** The data now updates four times per second (twice as fast as before).
*   **Code Refactoring:** Minor code adjustments and improvements for better maintainability and performance.

## [0.1.0] - 2025-01-28

### Added
* Initial Release.
