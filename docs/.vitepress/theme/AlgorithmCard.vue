<script setup lang="ts">
interface AlgorithmLink {
  href: string
  label: string
}

interface Props {
  title: string
  timeComplexity?: string
  spaceComplexity?: string
  description: string
  links?: AlgorithmLink[]
}

defineProps<Props>()
</script>

<template>
  <article class="wp-algorithm-card">
    <div class="wp-algorithm-card-header">
      <h3 class="wp-algorithm-card-title">{{ title }}</h3>
    </div>

    <div class="wp-algorithm-card-complexity">
      <ComplexityBadge v-if="timeComplexity" :value="timeComplexity" label="Time" />
      <ComplexityBadge v-if="spaceComplexity" :value="spaceComplexity" label="Space" />
    </div>

    <p class="wp-algorithm-card-desc">{{ description }}</p>

    <div v-if="links?.length" class="wp-algorithm-card-links">
      <BaseAwareLink
        v-for="link in links"
        :key="`${link.href}:${link.label}`"
        class="wp-pill-link"
        :href="link.href"
      >
        {{ link.label }}
      </BaseAwareLink>
    </div>
  </article>
</template>
