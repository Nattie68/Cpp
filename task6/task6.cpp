#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

struct Coordinates {
    double latitude;
    double longitude;
};

class ControlPoint {
public:
    virtual ~ControlPoint() = default;
    virtual std::string toString(size_t index) const = 0;
    virtual double penalty() const = 0;
};

class MandatoryCP : public ControlPoint {
    std::string name;
    Coordinates coords;
public:
    MandatoryCP(const std::string& n, const Coordinates& c) : name(n), coords(c) {}
    std::string toString(size_t index) const override {
        std::stringstream ss;
        ss << index << ". " << name << " (" << coords.latitude << ", " << coords.longitude << ") SU failed";
        return ss.str();
    }
    double penalty() const override { return 0.0; }
};

class OptionalCP : public ControlPoint {
    std::string name;
    Coordinates coords;
    double fine;
public:
    OptionalCP(const std::string& n, const Coordinates& c, double f) : name(n), coords(c), fine(f) {}
    std::string toString(size_t index) const override {
        std::stringstream ss;
        ss << index << ". " << name << " (" << coords.latitude << ", " << coords.longitude << ") penalty: " << fine << " hours";
        return ss.str();
    }
    double penalty() const override { return fine; }
};

class TrackBuilder {
public:
    virtual ~TrackBuilder() = default;
    virtual void addMandatory(const std::string& name, const Coordinates& coords) = 0;
    virtual void addOptional(const std::string& name, const Coordinates& coords, double fine) = 0;
    virtual void printResult() const = 0;
};

class TextTrackBuilder : public TrackBuilder {
    std::vector<std::unique_ptr<ControlPoint>> points;
public:
    void addMandatory(const std::string& name, const Coordinates& coords) override {
        points.push_back(std::make_unique<MandatoryCP>(name, coords));
    }
    void addOptional(const std::string& name, const Coordinates& coords, double fine) override {
        points.push_back(std::make_unique<OptionalCP>(name, coords, fine));
    }
    void printResult() const override {
        for (size_t i = 0; i < points.size(); ++i) {
            std::cout << points[i]->toString(i + 1) << std::endl;
        }
    }
};

class PenaltySumBuilder : public TrackBuilder {
    double totalPenalty = 0.0;
public:
    void addMandatory(const std::string&, const Coordinates&) override {}
    void addOptional(const std::string&, const Coordinates&, double fine) override {
        totalPenalty += fine;
    }
    void printResult() const override {
        std::cout << "Total penalty: " << totalPenalty << " hours" << std::endl;
    }
};

struct RoutePoint {
    std::string name;
    Coordinates coords;
    bool isMandatory;
    double fine;
};

class RouteDirector {
    TrackBuilder* builder;
public:
    void setBuilder(TrackBuilder* b) {
        builder = b;
    }
    
    void processRoute(const std::vector<RoutePoint>& route) {
        for (const auto& point : route) {
            if (point.isMandatory) {
                builder->addMandatory(point.name, point.coords);
            } else {
                builder->addOptional(point.name, point.coords, point.fine);
            }
        }
    }
};

int main() {
    std::vector<RoutePoint> track = {
        {"Start", {54.31, 48.08}, true, 0.0},
        {"Camp", {54.32, 48.11}, true, 0.0},
        {"River", {54.35, 48.15}, false, 3.0},
        {"Forest", {54.42, 48.25}, false, 1.5},
        {"Lake", {54.45, 48.30}, false, 5.0},
        {"Rock", {54.48, 48.35}, true, 0.0},
        {"Finish", {54.50, 48.40}, true, 0.0}
    };
    
    RouteDirector director;
    
    TextTrackBuilder textBuilder;
    director.setBuilder(&textBuilder);
    director.processRoute(track);
    
    textBuilder.printResult();
    
    PenaltySumBuilder penaltyBuilder;
    director.setBuilder(&penaltyBuilder);
    director.processRoute(track);
    
    penaltyBuilder.printResult();
    
    return 0;
}
