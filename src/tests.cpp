// Features
template <int dimensions, class T>
class AbstractFeature {};

template <int dimensions,
          class T,
          template <int dim, class P, class Group>
          class Feature,
          class Derived>
class FeatureGroup;

template <int dimensions, class Derived, class T, class Group>
class AbstractGroupedFeature : public AbstractFeature<dimensions, T> {
  friend Group;

 public:
  explicit AbstractGroupedFeature(Group* group = nullptr) : _group(nullptr) {
    if (group)
      group->add(static_cast<Derived*>(this));
  }

  ~AbstractGroupedFeature() {
    if (_group)
      _group->remove(static_cast<Derived*>(this));
  }

  /** @brief Group this feature belongs to */
  Group* group() { return _group; }

  /** @overload */
  const Group* group() const { return _group; }

 private:
  Group* _group;
};

// Groups
template <int dimensions, class T>
class AbstractFeatureGroup {};

template <int dimensions,
          class T,
          template <int dim, class P, class Group>
          class Feature,
          class Derived>
class FeatureGroup : public AbstractFeatureGroup<dimensions, T> {
  using GroupedFeature = Feature<dimensions, T, Derived>;
  using self = FeatureGroup<dimensions, T, Feature, Derived>;
  friend AbstractGroupedFeature<dimensions, GroupedFeature, T, FeatureGroup>;
  // using T = typename GroupedFeature::n;

 public:
  explicit FeatureGroup() = default;

  ~FeatureGroup() = default;

  self& add(GroupedFeature& feature) { return *this; }

  self& remove(GroupedFeature& feature) { return *this; }
};

template <int dimensions, class T, class Group>
class DrawableWithGroup
    : public AbstractGroupedFeature<dimensions,
                                    DrawableWithGroup<dimensions, T, Group>,
                                    T,
                                    Group> {
  using Parent = AbstractGroupedFeature<dimensions,
                                        DrawableWithGroup<dimensions, T, Group>,
                                        T,
                                        Group>;
  Group* drawables() { return Parent::group(); }

  /** @overload */
  const Group* drawables() const { return Parent::group(); }
};

template <int dimensions, class T, class Derived>
using DrawableGroup = FeatureGroup<dimensions, T, DrawableWithGroup, Derived>;

template <class Derived>
using DrawableGroup3D = DrawableGroup<3, float, Derived>;

// use DrawableGroup as default Grouping
template <int dimensions, class T>
using Drawable = DrawableWithGroup<dimensions, T, DrawableGroup<dimensions, T>>;

using Drawable3D = Drawable<3, float>;

class DrawableGroupDerived : public DrawableGroup3D<DrawableGroupDerived> {};

class DrawableDerived
    : public DrawableWithGroup<3, float, DrawableGroupDerived> {};

int main() {
  // DrawableGroup3D group{};
  // DrawableGroupDerived groupDerived{};
  DrawableDerived drawableDerived{};
  // Drawable3D drawable{};
  // group.add(drawable);
}
