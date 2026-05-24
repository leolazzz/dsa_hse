#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <cmath>

int char_count = 0;

int comp_str(const std::string& a, const std::string& b) {
	size_t i = 0;
	while (i < a.size() && i < b.size()) {
		char_count++;
		if (a[i] < b[i]) {
			return -1;
		}
		if (a[i] > b[i]) {
			return 1;
		}
		++i;
	}
	if (a.size() < b.size()) {
		return -1;
	}
	if (a.size() > b.size()) {
		return 1;
	}
	return 0;
}

int get_char(const std::string &s, size_t i) {
	++char_count;
	if (i < s.size()) {
		return (unsigned char)s[i];
	}
	return -1;
}

int triple_comp(const std::string& s, size_t i, int pivot) {
	int now = get_char(s, i);
	if (now < pivot) {
		return -1;
	}
	if (now > pivot) {
		return 1;
	}
	return 0;
}

int median(std::vector <std::string>& a, int l, int r) {
	int m = l + (r - l) / 2;
	if (comp_str(a[m], a[l]) == -1) {
		std::swap(a[m], a[l]);
	}
	if (comp_str(a[r], a[l]) == -1) {
		std::swap(a[r], a[l]);
	}
	if (comp_str(a[r], a[m]) == -1) {
		std::swap(a[r], a[m]);
	}
	return m;
}

int make_partion(std::vector <std::string> & a, int l, int r) {
	int piv_i = median(a, l, r);
	std::string piv = a[piv_i];
	std::swap(a[piv_i], a[r]);
	int i = l;
	for (int j = l; j < r; ++j) {
		if (comp_str(a[j], piv) <= 0) {
			std::swap(a[i], a[j]);
			++i;
		}
	}
	std::swap(a[i], a[r]);
	return i;
}

void quick_sort(std::vector <std::string>& a, int l, int r) {
	if (l >= r) {
		return;
	}
	int p = make_partion(a, l, r);
	quick_sort(a, l, p - 1);
	quick_sort(a, p + 1, r);
}

void quick_sort_st(std::vector <std::string> &a) {
	if (a.size() > 0) {
		quick_sort(a, 0, a.size() - 1);
	}
}

void quick_sort_ran(std::vector <std::string>& a, int l, int r) {
	if (l < r) {
		quick_sort(a, l, r);
	}
}

void merge(std::vector<std::string>& a, std::vector<std::string>& tmp, int l, int m, int r) {
	int i = l, j = m + 1, k = l;
	while (i <= m && j <= r) {
		if (comp_str(a[i], a[j]) == 1) {
			tmp[k] = a[j];
			++k;
			++j;
		} else {
			tmp[k] = a[i];
			++k;
			++i;
		}
	}
	while (i <= m) {
		tmp[k] = a[i];
		++k;
		++i;
	}
	while (j <= r) {
		tmp[k] = a[j];
		++k;
		++j;
	}
	for (i = l; i <= r; ++i) {
		a[i] = tmp[i];
	}
}

void merge_sort(std::vector<std::string>& a, std::vector<std::string>& tmp, int l, int r) {
	if (l >= r) {
		return;
	}
	int m = l + (r - l) / 2;
	merge_sort(a, tmp, l, m);
	merge_sort(a, tmp, m + 1, r);
	merge(a, tmp, l, m, r);;
}

void merge_sort_st(std::vector <std::string> &a) {
	if (a.size() <= 1) {
		return;
	}

	std::vector <std::string> tmp(a.size());
	merge_sort(a, tmp, 0, a.size() - 1);
}

void ternary_qsort(std::vector<std::string>& a, int l, int r, size_t d) {
	if (l >= r) {
		return;
	}
	size_t max = 0;
	for (int i = l; i <= r; ++i) {
		max = std::max(a[i].size(), max);
	}
	if (d >= max) {
		return;
	}

	int c1 = get_char(a[l], d);
	int c2 = get_char(a[l + (r - l) / 2], d);
	int c3 = get_char(a[r], d);
	int pivot = c1;
	if ((c1 <= c2 && c2 <= c3) || (c3 <= c2 && c2 <= c1)) {
		pivot = c2;
	} else if ((c2 <= c1 && c1 <= c3) || (c3 <= c1 && c1 <= c2)) {
		pivot = c1;
	} else {
		pivot = c3;
	}

	int i = l, less = l,  bigger = r;
	while (i <= bigger) {
		int cmp = triple_comp(a[i], d, pivot);
		if (cmp < 0) {
			std::swap(a[less], a[i]);
			++less;
			++i;
		} else if (cmp > 0) {
			std::swap(a[i], a[bigger]);
			--bigger;
		} else {
			++i;
		}
	}
	ternary_qsort(a, l, less - 1, d);
	ternary_qsort(a, less, bigger, d + 1);
	ternary_qsort(a, bigger + 1, r, d);
}

void ternary_quicksort_st(std::vector<std::string>& a) {
	if (a.size() > 0) {
		ternary_qsort(a, 0, a.size() - 1, 0);
	}
}

void merge_lcp(std::vector<std::string>& a, std::vector<int>& lcp, int l, int m, int r) {
	int n1 = m - l + 1, n2 = r - m;
	std::vector<std::string> L(n1), R(n2);
	for (int i = 0; i < n1; ++i) {
		L[i] = a[l + i];
	}
	for (int j = 0; j < n2; ++j) {
		R[j] = a[m + 1 + j];
	}

	std::vector<int> lcpL(n1 - 1), lcpR(n2 - 1);
	for (int i = 0; i < n1 - 1; ++i) {
		lcpL[i] = lcp[l + i];
	}
	for (int j = 0; j < n2 - 1; ++j) {
		lcpR[j] = lcp[m + 1 + j];
	}

	std::vector<std::string> merged(n1 + n2);
	std::vector<int> mergedLcp(n1 + n2 - 1);
	int i = 0, j = 0, k = 0;
	int lcp_left = 0, lcp_right = 0;
	bool last_left = 0;

	while (i < n1 && j < n2) {
		int cmp, lcp_ij;
		if (k == 0) {
			const std::string& a = L[i], &b = R[j];
			int p = 0;
			while (p < a.size() && p < b.size()) {
				char_count++;
				if (a[p] != b[p]) {
					break;
				}
				++p;
			}
			lcp_ij = p;
			if (p == a.size() && p == b.size()) {
				cmp = 0;
			} else if (p == a.size()) {
				cmp = -1;
			} else if (p == b.size()) {
				cmp = 1;
			} else {
				cmp = (a[p] < b[p]) ? -1 : 1;
			}
		} else {
			if (lcp_left > lcp_right) {
				cmp = -1;
				lcp_ij = lcp_right;
			} else if (lcp_left < lcp_right) {
				cmp = 1;
				lcp_ij = lcp_left;
			} else {
				const std::string& a = L[i], & b = R[j];
				int p = lcp_left;
				while (p < a.size() && p < b.size()) {
					char_count++;
					if (a[p] != b[p]) {
						break;
					}
					++p;
				}
				lcp_ij = p;
				if (p == a.size() && p == b.size()) {
					cmp = 0;
				} else if (p == a.size()) {
					cmp = -1;
				} else if (p == b.size()) {
					cmp = 1;
				} else {
					cmp = 1;
					if (a[p] < b[p]) {
						cmp = -1;
					}
				}
			}
		}

		
		if (cmp <= 0) {
			merged[k] = L[i];
			if (k > 0) {
				mergedLcp[k - 1] = lcp_left;
			}
			if (i + 1 < n1) {
				lcp_left = lcpL[i];
			} else {
				lcp_left = 0;
			}
			lcp_right = lcp_ij;
			last_left = 1;
			++i;
		} else {
			merged[k] = R[j];
			if (k > 0) {
				mergedLcp[k - 1] = lcp_right;
			}
			if (j + 1 < n2) {
				lcp_right = lcpR[j];
			} else {
				lcp_right = 0;
			}
			lcp_left = lcp_ij;
			last_left = 0;
			++j;
		}
		++k;
	}

	while (i < n1) {
		merged[k] = L[i];
		if (k > 0) {
			if (last_left) {
				if (i > 0) {
					mergedLcp[k - 1] = lcpL[i - 1];
				} else {
					mergedLcp[k - 1] = 0;
				}
			} else {
				mergedLcp[k - 1] = lcp_left;
			}
		}
		++i;
		++k;
		last_left = 1;
	}

	while (j < n2) {
		merged[k] = R[j];
		if (k > 0) {
			if (!last_left) {
				if (j > 0) {
					mergedLcp[k - 1] = lcpR[j - 1];
				} else {
					mergedLcp[k - 1] = 0;
				}
			} else {
				mergedLcp[k - 1] = lcp_right;
			}
		}
		++j;
		++k;
		last_left = 0;
	}
	for (int t = 0; t < n1 + n2; ++t) {
		a[l + t] = merged[t];
	}
	for (int t = 0; t < n1 + n2 - 1; ++t) {
		lcp[l + t] = mergedLcp[t];
	}
}

void mergesort_lcp(std::vector<std::string>& a, std::vector<int>& lcp, int l, int r) {
	if (l >= r) {
		return;
	}
	int m = l + (r - l) / 2;
	mergesort_lcp(a, lcp, l, m);
	mergesort_lcp(a, lcp, m + 1, r);
	merge_lcp(a, lcp, l, m, r);
}

void mergesort_lcp_st(std::vector<std::string>&a) {
	if (a.size() <= 1) {
		return;
	}
	std::vector<int> lcp(a.size(), 0);
	mergesort_lcp(a, lcp, 0, a.size() - 1);
}

void msd_sort_noq(std::vector<std::string>& a, std::vector<std::string>& aux, int l, int r, int d) {
	if (r - l + 1 <= 1) {
		return;
	}
	std::vector<int> count(257, 0);
	for (int i = l; i <= r; ++i) {
		int bin;
		if (d < a[i].size()) {
			bin = (unsigned char)a[i][d] + 1;
			char_count++;
		} else {
			bin = 0;
		}
		++count[bin];
	}
	std::vector <int> pos(257);
	pos[0] = l;
	for (int b = 1; b <= 256; ++b) {
		pos[b] = pos[b - 1] + count[b - 1];
	}
	for (int i = l; i <= r; ++i) {
		int bin;
		if (d < a[i].size()) {
			bin = (unsigned char)a[i][d] + 1;
			char_count++;
		} else {
			bin = 0;
		}
		aux[pos[bin]++] = a[i];
	}
	for (int i = l; i <= r; ++i) {
		a[i] = aux[i];
	}
	if (count[0] > 1) {
		quick_sort_ran(a, l, l + count[0] - 1);
	}
	int start = l + count[0];
	for (int b = 1; b <= 256; ++b) {
		if (count[b] > 0) {
			msd_sort_noq(a, aux, start, start + count[b] - 1, d + 1);
			start += count[b];
		}
	}
}

void msd_radix_sort_noq_st(std::vector<std::string>& arr) {
	if (arr.size() <= 1) {
		return;
	}
	std::vector<std::string> aux(arr.size());
	msd_sort_noq(arr, aux, 0, arr.size() - 1, 0);
}

void msd_sort_q(std::vector<std::string>& a, std::vector<std::string>& aux, int l, int r, int d) {
	if (r - l + 1 <= 1) {
		return;
	}
	if (r - l + 1 < 74) {
		ternary_qsort(a, l, r, d);
		return;
	}
	std::vector <int> count(257, 0);
	for (int i = l; i <= r; ++i) {
		int bin;
		if (d < a[i].size()) {
			bin = (unsigned char)a[i][d] + 1;
			char_count++;
		} else {
			bin = 0;
		}
		++count[bin];
	}
	std::vector <int> pos(257);
	pos[0] = l;
	for (int b = 1; b <= 256; ++b) {
		pos[b] = pos[b - 1] + count[b - 1];
	}
	for (int i = l; i <= r; ++i) {
		int bin;
		if (d < a[i].size()) {
			bin = (unsigned char)a[i][d] + 1;
			char_count++;
		} else {
			bin = 0;
		}
		aux[pos[bin]++] = a[i];
	}
	for (int i = l; i <= r; ++i) {
		a[i] = aux[i];
	}
	int bin0_end = l + count[0] - 1;
	if (count[0] > 1) {
		quick_sort_ran(a, l, bin0_end);
	}
	int start = l + count[0];
	for (int b = 1; b <= 256; ++b) {
		if (count[b] > 0) {
			msd_sort_q(a, aux, start, start + count[b] - 1, d + 1);
			start += count[b];
		}
	}
}

void msd_radix_sort_q_st(std::vector<std::string>& arr) {
	if (arr.size() <= 1) {
		return;
	}
	std::vector<std::string> aux(arr.size());
	msd_sort_q(arr, aux, 0, arr.size() - 1, 0);
}

class StringGenerator {
public:
	static const std::string charset;
	static std::string gen_random_string(std::mt19937& rng, std::uniform_int_distribution<int>& len_dist,
		std::uniform_int_distribution<int>& char_dist) {
		int len = len_dist(rng);
		std::string s(len, ' ');
		for (int i = 0; i < len; ++i) {
			s[i] = charset[char_dist(rng)];
		}
		return s;
	}

	static std::vector<std::string> generate_random(int n) {
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> len_dist(10, 200);
		std::uniform_int_distribution<int> char_dist(0, (int)charset.size() - 1);
		std::vector<std::string> arr(n);
		for (int i = 0; i < n; ++i) {
			arr[i] = gen_random_string(rng, len_dist, char_dist);
		}
		return arr;
	}

	static std::vector<std::string> generate_sorted(int n) {
		std::vector<std::string> arr = generate_random(n);
		std::sort(arr.begin(), arr.end());
		return arr;
	}

	static std::vector<std::string> generate_reverse_sorted(int n) {
		std::vector<std::string> arr = generate_sorted(n);
		reverse(arr.begin(), arr.end());
		return arr;
	}

	static std::vector<std::string> generate_nearly_sorted(int n) {
		std::vector<std::string> arr = generate_sorted(n);
		int swaps = std::max(1, n / 20);
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> idx(0, n - 1);
		for (int i = 0; i < swaps; ++i) {
			int a = idx(rng), b = idx(rng);
			swap(arr[a], arr[b]);
		}
		return arr;
	}


	static std::vector<std::string> base_random() {
		return generate_random(3000);
	}
	static std::vector<std::string> base_reverse() {
		std::vector<std::string> arr = generate_random(3000);
		sort(arr.begin(), arr.end());
		reverse(arr.begin(), arr.end());
		return arr;
	}
	static std::vector<std::string> base_nearly() {
		std::vector<std::string> arr = generate_random(3000);
		std::sort(arr.begin(), arr.end());
		int swaps = std::max(1, 3000 / 20);
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> idx(0, 2999);
		for (int i = 0; i < swaps; ++i) {
			std::swap(arr[idx(rng)], arr[idx(rng)]);
		}
		return arr;
	}
};

std::string make_charset() {
	std::string s;
	for (char c = 'A'; c <= 'Z'; ++c) {
		s.push_back(c);
	}
	for (char c = 'a'; c <= 'z'; ++c) {
		s.push_back(c);
	}
	for (char c = '0'; c <= '9'; ++c) {
		s.push_back(c);
	}
	s += "!@#%:;^&*()-.";
	return s;
}

const std::string StringGenerator::charset = make_charset();

struct SortResult {
	double avg_time_ms;
	unsigned long long comparisons;
};

typedef void (*SortFunc)(std::vector<std::string>&);

class StringSortTester {
public:
	static SortResult measure(SortFunc sort_func, const std::vector<std::string>& base, int runs = 5) {
		unsigned long long total_comps = 0;
		double total_time = 0.0;
		for (int r = 0; r < runs; ++r) {
			std::vector<std::string> arr = base;
			char_count = 0;
			auto start = std::chrono::high_resolution_clock::now();
			sort_func(arr);
			auto end = std::chrono::high_resolution_clock::now();
			total_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			total_comps += char_count;
		}
		return { total_time / runs / 1000.0, total_comps / runs };
	}
};

int main() {
	const int N = 500;
	auto base_random = StringGenerator::base_random();
	std::vector<std::string> arr_random(base_random.begin(), base_random.begin() + N);

	SortResult qs = StringSortTester::measure(quick_sort_st, arr_random);
	std::cout << "Time(quick sort): " << qs.avg_time_ms << " ms, comparisons: " << qs.comparisons << "\n\n";
	
	SortResult ms = StringSortTester::measure(merge_sort_st, arr_random);
	std::cout << "Time(merge sort): " << ms.avg_time_ms << " ms, comparisons: " << ms.comparisons << "\n\n";

	SortResult tq = StringSortTester::measure(ternary_quicksort_st, arr_random);
	std::cout << "Time(ter quick sort): " << tq.avg_time_ms << " ms, comparisons: " << tq.comparisons << "\n\n";

	SortResult sm = StringSortTester::measure(mergesort_lcp_st, arr_random);
	std::cout << "Time(merge lcp sort): " << sm.avg_time_ms << " ms, comparisons: " << sm.comparisons << "\n\n";

	SortResult msd = StringSortTester::measure(msd_radix_sort_noq_st, arr_random);
	std::cout << "Time(radix no quick): " << msd.avg_time_ms << " ms, comparisons: " << msd.comparisons << "\n\n";

	SortResult msdc = StringSortTester::measure(msd_radix_sort_q_st, arr_random);
	std::cout << "Time(radix quick sort): " << msdc.avg_time_ms << " ms, comparisons: " << msdc.comparisons << "\n";

	return 0;
}