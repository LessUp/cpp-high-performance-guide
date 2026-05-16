<script setup lang="ts">
interface MetricItem {
  value: string
  label: string
}

defineProps<{
  items: MetricItem[]
  label?: string
}>()
</script>

<template>
  <section class="wp-metric-strip">
    <div v-if="label" class="wp-metric-strip__label">{{ label }}</div>
    <div class="wp-metric-strip__grid">
      <div v-for="item in items" :key="`${item.value}-${item.label}`" class="wp-metric-strip__item">
        <span class="wp-metric-strip__value">{{ item.value }}</span>
        <span class="wp-metric-strip__text">{{ item.label }}</span>
      </div>
    </div>
  </section>
</template>
