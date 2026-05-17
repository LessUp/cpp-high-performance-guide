<script setup lang="ts">
import { computed } from 'vue'

interface Props {
  value: string
  label?: string
}

const props = defineProps<Props>()

const className = computed(() => {
  const v = props.value.toLowerCase().trim()
  if (v === 'o(1)' || v === 'o(1)') return 'wp-complexity-badge--constant'
  if (v.includes('log')) return 'wp-complexity-badge--logarithmic'
  if (v === 'o(n)' || v === 'o(n)') return 'wp-complexity-badge--linear'
  if (v.includes('n log') || v.includes('nlog')) return 'wp-complexity-badge--linearithmic'
  if (v.includes('n²') || v.includes('n^2') || v.includes('n*n')) return 'wp-complexity-badge--quadratic'
  if (v.includes('2^n') || v.includes('exp')) return 'wp-complexity-badge--exponential'
  return 'wp-complexity-badge--linear'
})
</script>

<template>
  <span class="wp-complexity-badge" :class="className" :title="label ? `${label}: ${value}` : value">
    <template v-if="label">{{ label }}&nbsp;</template>{{ value }}
  </span>
</template>
