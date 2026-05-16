<script setup lang="ts">
interface ReferenceItem {
  title: string
  href: string
  meta?: string
  description?: string
}

defineProps<{
  items: ReferenceItem[]
  title?: string
}>()
</script>

<template>
  <section class="wp-reference-list">
    <h2 v-if="title" class="wp-reference-list__title">{{ title }}</h2>
    <ul class="wp-reference-list__items">
      <li v-for="item in items" :key="`${item.href}-${item.title}`" class="wp-reference-list__item">
        <span v-if="item.meta" class="wp-reference-list__meta">{{ item.meta }}</span>
        <a class="wp-reference-list__link" :href="item.href">{{ item.title }}</a>
        <span v-if="item.description" class="wp-reference-list__description">{{ item.description }}</span>
      </li>
    </ul>
  </section>
</template>
