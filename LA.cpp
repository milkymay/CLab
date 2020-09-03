#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define NaN Number("--")

using namespace std;

class Number {
public:
    explicit Number(string a) {
        int beg = 0;
        if (a[0] == '-') {
            sign = 1;
            beg++;
        } else {
            sign = 0;
        }
        vector<int> num;
        for (int i = beg; i < a.size(); i++) {
            try {
                num.push_back(a[i] - '0');
            } catch (...) {
                cout << "push_back error when creating Number\n";
                throw;
            }
        }
        reverse(num.begin(), num.end());
        digits = num;
        shrink_to_fit();
    }

    Number(vector<int> digits, int sign) {
        this->digits = digits;
        this->sign = sign;
        shrink_to_fit();
    }

    ~Number() = default;

    void print() {
        if (*this == NaN) {
            cout << "NaN\n";
            return;
        }
        if (sign) {
            cout << "-";
        }
        for (int i = (int) digits.size() - 1; i >= 0; i--) {
            cout << get_digit(i);
        }
        cout << "\n";
    }


    int operator<(const Number &b) const {
        return (compare(b) == -1);
    }

    int operator<=(const Number &b) const {
        return (compare(b) <= 0);
    }

    int operator>(const Number &b) const {
        return (compare(b) == 1);
    }

    int operator>=(const Number &b) const {
        return (compare(b) >= 0);
    }

    int operator==(const Number &b) const {
        return (compare(b) == 0);
    }

    int operator!=(const Number &b) const {
        return (compare(b) != 0);
    }


    Number operator+(const Number &b) const {
        if (sign != b.sign) {
            if (sign) {
                const Number a(digits, !sign);
                return b - a;
            } else {
                const Number c(b.digits, !b.sign);
                return *this - c;
            }
        }
        vector<int> res;
        int carry = 0;
        for (int i = 0; i < max(digits.size(), b.digits.size()); i++) {
            int cur = get_digit(i) + b.get_digit(i) + carry;
            try {
                res.push_back(cur % 10);
            } catch (...) {
                cout << "push_back error when creating Number\n";
                throw;
            }
            carry = cur / 10;
        }
        if (carry != 0) {
            try {
                res.push_back(carry);
            } catch (...) {
                cout << "push_back error when creating Number\n";
                throw;
            }
        }
        return Number(res, sign);
    }

    Number operator-(const Number &b) const {
        if (sign != b.sign) {
            if (sign) {
                const Number c(b.digits, !b.sign);
                return *this + c;
            } else {
                const Number c(b.digits, !b.sign);
                return *this + c;
            }
        }
        if ((!sign && (*this < b)) || (sign && (*this > b))) {
            Number tmp = (b - *this);
            Number res(tmp.digits, !tmp.sign);
            return res;
        }
        vector<int> res;
        int take = 0;
        for (int i = 0; i < digits.size(); i++) {
            int cur = get_digit(i) - b.get_digit(i) - take;
            take = 0;
            if (cur < 0) {
                cur += 10;
                take = 1;
            }
            try {
                res.push_back(cur);
            } catch (...) {
                cout << "push_back error when creating Number\n";
                throw;
            }
        }
        return Number(res, sign);
    }

    Number operator*(const Number &b) const {
        if (zero() || b.zero()) {
            return Number("0");
        }
        vector<int> res;
        try {
            res.resize(digits.size() + b.digits.size() + 1);
        } catch (...) {
            cout << "Resize error in multiplication\n";
            throw;
        }
        for (int i = 0; i < digits.size(); i++) {
            for (int j = 0; j < b.digits.size(); j++) {
                res[i + j] += get_digit(i) * b.get_digit(j);
            }
        }
        for (int i = 0; i < res.size(); i++) {
            res[i + 1] += res[i] / 10;
            res[i] %= 10;
        }
        return Number(res, sign != b.sign);
    }

    Number operator/(const Number &b) const {
        if (zero()) {
            return Number("0");
        }
        if (b.zero()) {
//            cout << "Tried to divide by zero";
            return NaN;
        }
        Number l("0"), r(digits, sign);
        while (r >= l) {
            Number mid = (l + r).divide(2);
            if ((*this - b < b * mid) && (b * mid <= *this)) {
                return mid;
            }
            if ((mid * b) < *this) {
                l = mid;
            } else {
                r = mid;
            }
        }
        return l;
    }

    Number operator%(const Number &b) const {
        if (zero()) {
            return Number("0");
        }
        if (b.zero()) {
//            cout << "Tried to divide by zero";
            return NaN;
        }
        Number l("-1"), r(digits, sign);
        Number mid = (l + r).divide(2);
        while (r >= l) {
            mid = (l + r).divide(2);
            if ((*this - b < b * mid) && (b * mid <= *this)) {
                return *this - mid * b;
            }
            if ((mid * b) < *this) {
                l = mid;
            } else {
                r = mid;
            }
        }
        return l;
    }

    Number divide(long long b) const {
        if (b == 0) {
            return NaN;
        }
        Number res(digits, sign);
        int carry = 0;
        for (int i = (int) digits.size() - 1; i >= 0; i--) {
            long long cur = digits[i] + carry * 1ll * 10;
            res.digits[i] = (cur / b);
            carry = (int) (cur % b);
        }
        res.shrink_to_fit();
        return res;
    }

    Number root() const {
        if (sign) {
            return NaN;
        }
        Number l("0"), r(digits, sign);
        Number mid = (l + r).divide(2);
        while (r >= l) {
            mid = (l + r).divide(2);
            if (((mid + Number("1")) * (mid + Number("1")) > *this) && (*this >= mid * mid)) {
                return mid;
            }
            if ((mid * mid) < *this) {
                l = mid;
            } else {
                r = mid;
            }
        }
        return l;
    }


private:
    int get_digit(int i) const {
        if (0 <= i && i < digits.size()) {
            return digits[i];
        } else {
            return 0;
        }
    }

    bool zero() const {
        return digits.size() == 1 && digits[0] == 0;
    }

    Number &operator=(Number const &) = default;

    int same_sign_compare(const Number &b) const {
        for (int i = (int) max(digits.size(), b.digits.size()) - 1; i >= 0; i--) {
            if (get_digit(i) < b.get_digit(i)) {
                return -1;
            } else if (get_digit(i) > b.get_digit(i)) {
                return 1;
            }
        }
        return 0;
    }

    int compare(const Number &b) const {
        if (sign && !b.sign) return -1;
        if (!sign && b.sign) return 1;
        if (!sign && !b.sign) return same_sign_compare(b);
        else return -same_sign_compare(b);
    }

    void shrink_to_fit() {
        int sz = (int) digits.size() - 1;
        for (int i = sz; i > 0; i--) {
            if (digits[i] == 0) {
                digits.pop_back();
            } else {
                break;
            }
        }
        if (digits.size() == 1 && digits[0] == 0) {
            sign = 0;
        }
    }

    vector<int> digits;
    int sign;
};

int main(int args, char *arg[]) {
    int code = 2;
    if (args != 3) {
        cout << "Expected string: \"lab4 <input file name> <output file name>\"";
        return code;
    }
    FILE *in = freopen(arg[1], "r", stdin);
    if (!in) {
        cout << "Couldn't open input file";
        return code;
    }
    FILE *out = freopen(arg[2], "w", stdout);
    if (!out) {
        cout << "Couldn't open output file";
        fclose(in);
        return code;
    }
    /// files OK
    string num1, num2, sign;
    cin >> num1 >> sign;
    Number a = Number(num1);
    if (sign == "#") {
        a.root().print();
        return 0;
    }
    cin >> num2;
    Number b = Number(num2);
    if (sign == ">") {
        cout << (a > b) << "\n";
    } else if (sign == ">=") {
        cout << (a >= b) << "\n";
    } else if (sign == "<") {
        cout << (a < b) << "\n";
    } else if (sign == "<=") {
        cout << (a <= b) << "\n";
    } else if (sign == "==") {
        cout << (a == b) << "\n";
    } else if (sign == "!=") {
        cout << (a != b) << "\n";
    } else if (sign == "+") {
        (a + b).print();
    } else if (sign == "-") {
        (a - b).print();
    } else if (sign == "*") {
        (a * b).print();
    } else if (sign == "/") {
        (a / b).print();
    } else if (sign == "%") {
        (a % b).print();
    } else {
        cout << "Unknown sign \"" << sign << "\"";
    }
    fclose(in);
    fclose(out);
    return 0;
}