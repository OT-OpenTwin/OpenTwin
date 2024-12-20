#pragma once

class ScanOptions {
public:
	enum Operation {
		UpdateFileComment
	};

	ScanOptions(Operation _operation);

	Operation getOperation(void) const { return m_operation; };

private:
	Operation m_operation;

	ScanOptions() = delete;
	ScanOptions(const ScanOptions&) = delete;
	ScanOptions(ScanOptions&&) = delete;
	ScanOptions& operator = (const ScanOptions&) = delete;
	ScanOptions& operator = (ScanOptions&&) = delete;
};