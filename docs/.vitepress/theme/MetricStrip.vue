<script setup lang="ts">
type MetricItem = {
  value: string
  label: string
}

defineProps<{
  items: MetricItem[]
  ariaLabel: string
}>()
</script>

<template>
  <section class="wp-metric-strip" :aria-label="ariaLabel">
    <div
      v-for="item in items"
      :key="`${item.value}:${item.label}`"
      class="wp-metric-strip-item"
    >
      <strong>{{ item.value }}</strong>
      <span>{{ item.label }}</span>
    </div>
  </section>
</template>
