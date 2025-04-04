/**
 * @file addons/cpp/impl/iter.hpp
 * @brief Iterator implementation.
 */

#pragma once

namespace flecs
{

inline flecs::entity iter::system() const {
    return flecs::entity(iter_->world, iter_->system);
}

inline flecs::entity iter::event() const {
    return flecs::entity(iter_->world, iter_->event);
}

inline flecs::id iter::event_id() const {
    return flecs::id(iter_->world, iter_->event_id);
}

inline flecs::world iter::world() const {
    return flecs::world(iter_->world);
}

inline flecs::entity iter::entity(size_t row) const {
    ecs_assert(row < static_cast<size_t>(iter_->count), 
        ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
    return flecs::entity(iter_->world, iter_->entities[row]);
}

inline flecs::entity iter::src(int8_t index) const {
    return flecs::entity(iter_->world, ecs_field_src(iter_, index));
}

inline flecs::id iter::id(int8_t index) const {
    return flecs::id(iter_->world, ecs_field_id(iter_, index));
}

inline flecs::id iter::pair(int8_t index) const {
    flecs::id_t id = ecs_field_id(iter_, index);
    ecs_check(ECS_HAS_ID_FLAG(id, PAIR), ECS_INVALID_PARAMETER, NULL);
    return flecs::id(iter_->world, id);
error:
    return flecs::id();
}

inline flecs::type iter::type() const {
    return flecs::type(iter_->world, ecs_table_get_type(iter_->table));
}

inline flecs::table iter::table() const {
    return flecs::table(iter_->real_world, iter_->table);
}

inline flecs::table iter::other_table() const {
    return flecs::table(iter_->real_world, iter_->other_table);
}

inline flecs::table_range iter::range() const {
    return flecs::table_range(iter_->real_world, iter_->table, 
        iter_->offset, iter_->count);
}

template <typename T, typename A,
    typename std::enable_if<std::is_const<T>::value, void>::type*>
inline flecs::field<A> iter::field(int8_t index) const {
    ecs_assert(!(iter_->flags & EcsIterCppEach) || 
               ecs_field_src(iter_, index) != 0, ECS_INVALID_OPERATION,
        "cannot .field from .each, use .field_at<%s>(%d, row) instead",
            _::type_name<T>(), index);
    return get_field<A>(index);
}

template <typename T, typename A,
    typename std::enable_if<
        std::is_const<T>::value == false, void>::type*>
inline flecs::field<A> iter::field(int8_t index) const {
    ecs_assert(!(iter_->flags & EcsIterCppEach) || 
               ecs_field_src(iter_, index) != 0, ECS_INVALID_OPERATION,
        "cannot .field from .each, use .field_at<%s>(%d, row) instead",
            _::type_name<T>(), index);
    ecs_assert(!ecs_field_is_readonly(iter_, index),
        ECS_ACCESS_VIOLATION, NULL);
    return get_field<A>(index);
}

inline flecs::entity iter::get_var(int var_id) const {
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
    return flecs::entity(iter_->world, ecs_iter_get_var(iter_, var_id));
}

/** Get value of variable by name.
 * Get value of a query variable for current result.
 */
inline flecs::entity iter::get_var(const char *name) const {
    const flecs::query_t *q = iter_->query;

    int var_id = ecs_query_find_var(q, name);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
    return flecs::entity(iter_->world, ecs_iter_get_var(iter_, var_id));
}

template <typename Func>
void iter::targets(int8_t index, const Func& func) {
    ecs_assert(iter_->table != nullptr, ECS_INVALID_OPERATION, NULL);
    ecs_assert(index < iter_->field_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_field_is_set(iter_, index), ECS_INVALID_PARAMETER, NULL);
    const ecs_type_t *table_type = ecs_table_get_type(iter_->table);
    const ecs_table_record_t *tr = iter_->trs[index];
    int32_t i = tr->index, end = i + tr->count;
    for (; i < end; i ++) {
        ecs_id_t id = table_type->array[i];
        ecs_assert(ECS_IS_PAIR(id), ECS_INVALID_PARAMETER, 
            "field does not match a pair");
        flecs::entity tgt(iter_->world, 
            ecs_pair_second(iter_->real_world, id));
        func(tgt);
    }
}

} // namespace flecs
